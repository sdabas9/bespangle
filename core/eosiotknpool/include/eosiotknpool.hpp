#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT mycontract : public contract {
public:
    using contract::contract;

    // Add balance action (triggered by eosio.token transfer)
    [[eosio::on_notify("eosio.token::transfer")]]
    void addbalance(name from, name to, asset quantity, std::string memo);
    // Allocate asset to a round and badge
    ACTION allocate(name org, name round, name badge, asset amount);
    // Withdraw funds from the deposit table
    ACTION withdraw(name org, asset amount); 

    ACTION dynamic(name org, name round, name badge, name account, uint64_t portion); 

    ACTION fixed(name org, name round, name badge, name account, uint64_t amount_val);
private:

    TABLE deposit {
        name org;
        asset total_deposit;
        asset total_allocated;

        auto primary_key() const { return org.value; }
    };
    typedef multi_index<"deposit"_n, deposit> deposit_table;

    // Table for pool balance
    TABLE poolbalance {
        uint64_t id;
        name round;
        name badge;
        asset pool_allocation;
        asset pool_distributed;

        auto primary_key() const { return id; }
        uint128_t by_round_and_badge() const { return (uint128_t(round.value) << 64) | badge.value; }
    };
    typedef multi_index<"poolbalance"_n, poolbalance,
            indexed_by<"byroundbadge"_n, const_mem_fun<poolbalance, uint128_t, &poolbalance::by_round_and_badge>>> poolbalance_table;
};
