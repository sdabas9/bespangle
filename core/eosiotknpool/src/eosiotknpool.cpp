#include <eosiotknpool.hpp>


void eosiotknpool::addbalance(name from, name to, asset quantity, std::string memo) {
    // Ensure that this contract is the receiver
    if (to != get_self()) return;

    // Ensure that the memo contains an org name
    name org(memo);

    deposit_table deposits(get_self(), get_self().value);
    auto dep_itr = deposits.find(org.value);

    // If the org doesn't exist, create a new entry. Otherwise, update the existing one.
    if (dep_itr == deposits.end()) {
        deposits.emplace(get_self(), [&](auto& row) {
            row.org = org;
            row.total_deposit = quantity;
            row.total_allocated = asset(0, quantity.symbol);
        });
    } else {
        deposits.modify(dep_itr, same_payer, [&](auto& row) {
            row.total_deposit += quantity;
        });
    }
}
    // Allocate asset to a round and badge
ACTION eosiotknpool::allocate(name org, name round, name badge, asset amount) {
    require_auth(org);

    deposit_table deposits(get_self(), get_self().value);
    auto dep_itr = deposits.find(org.value);
    eosio::check(dep_itr != deposits.end(), "Organization not found");
    eosio::check(dep_itr->total_deposit - dep_itr->total_allocated >= amount, "Insufficient balance to allocate");

    // Update the deposit table
    deposits.modify(dep_itr, same_payer, [&](auto& row) {
        row.total_allocated += amount;
    });

    // Update the poolbalance table
    poolbalance_table pools(get_self(), get_self().value);
    auto idx = pools.get_index<"byroundbadge"_n>();
    auto pool_itr = idx.find((uint128_t(round.value) << 64) | badge.value);

    if (pool_itr == idx.end()) {
        pools.emplace(get_self(), [&](auto& row) {
            row.id = pools.available_primary_key();
            row.round = round;
            row.badge = badge;
            row.pool_allocation = amount;
            row.pool_distributed = asset(0, amount.symbol);
        });
    } else {
        pools.modify(*pool_itr, same_payer, [&](auto& row) {
            row.pool_allocation += amount;
        });
    }
}

// Withdraw funds from the deposit table
ACTION eosiotknpool::withdraw(name org, asset amount) {
    require_auth(org);

    deposit_table deposits(get_self(), get_self().value);
    auto dep_itr = deposits.find(org.value);
    eosio::check(dep_itr != deposits.end(), "Organization not found");
    eosio::check(dep_itr->total_deposit - dep_itr->total_allocated >= amount, "Insufficient balance to withdraw");

    // Update the deposit table
    deposits.modify(dep_itr, same_payer, [&](auto& row) {
        row.total_deposit -= amount;
    });

    // Send the withdrawn funds back to the org
    action(
        permission_level{get_self(), "active"_n},
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(get_self(), org, amount, std::string("Withdrawal from contract"))
    ).send();
}

void eosiotknpool::dynamic(name org, name round, name badge, name account, uint64_t portion, name notification_contract) {
    
    eosio::check(portion <= 1000000, "Invalid portion value. It should not exceed 1000000 (representing 100%).");

    poolbalance_table pools(get_self(), get_self().value);
    auto idx = pools.get_index<"byroundbadge"_n>();
    auto pool_itr = idx.find((uint128_t(round.value) << 64) | badge.value);
    eosio::check(pool_itr != idx.end(), "Pool for the specified round and badge not found.");

    asset amount_to_transfer = asset((pool_itr->pool_allocation.amount * portion) / 1000000, pool_itr->pool_allocation.symbol);
    eosio::check(pool_itr->pool_allocation - pool_itr->pool_distributed >= amount_to_transfer, "Insufficient balance in pool to transfer.");

    // Update the poolbalance table
    pools.modify(*pool_itr, same_payer, [&](auto& row) {
        row.pool_distributed += amount_to_transfer;
    });

    // Send the calculated amount to the specified account
    action(
        permission_level{get_self(), "active"_n},
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(get_self(), account, amount_to_transfer, std::string("Dynamic allocation from contract"))
    ).send();
}

void eosiotknpool::fixed(name org, name round, name badge, name account, uint64_t amount_val, name notification_contract) {
    
    poolbalance_table pools(get_self(), get_self().value);
    auto idx = pools.get_index<"byroundbadge"_n>();
    auto pool_itr = idx.find((uint128_t(round.value) << 64) | badge.value);
    eosio::check(pool_itr != idx.end(), "Pool for the specified round and badge not found.");
    asset amount = asset(amount_val, pool_itr->pool_allocation.symbol);
    eosio::check(pool_itr->pool_allocation - pool_itr->pool_distributed >= amount, "Insufficient balance in pool to transfer.");

    // Update the poolbalance table
    pools.modify(*pool_itr, same_payer, [&](auto& row) {
        row.pool_distributed += amount;
    });

    // Send the specified amount to the provided account
    action(
        permission_level{get_self(), "active"_n},
        "eosio.token"_n, "transfer"_n,
        std::make_tuple(get_self(), account, amount, std::string("Fixed allocation from contract"))
    ).send();
}

