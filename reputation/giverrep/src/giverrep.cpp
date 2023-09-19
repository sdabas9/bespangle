#include <giverrep.hpp>


void giverrep::notifyachiev(
    name org,
    name badge,
    name account,
    name from,
    uint64_t count,
    string memo,
    vector<name> notify_accounts) {
    emissions_table emissions(_self, org.value);
    auto existing_emission = emissions.find(badge.value);

    // Ensure that the emission does not already exist
    eosio::check(existing_emission != emissions.end(), "Giver rep badge not setup");
    name giver_rep_badge = existing_emission->source_account_rep_badge;
    name emit_badge = existing_emission->emit_badge;

    keystats_table keystats(name(STATS_CONTRACT_NAME), org.value);
    auto keystats_itr = keystats.find(giver_rep_badge.value);
    eosio::check(keystats_itr != keystats.end(), "Stats not setup for givers rep badge");
    uint64_t mean_balance = keystats_itr->mean;

    uint64_t source_balance = account_balance(org, from, giver_rep_badge);
    uint64_t emit_amount;

    if ((source_balance * 100) % mean_balance == 0) {
        emit_amount = source_balance * 100 / mean_balance;
    } else {
        emit_amount = 1 + (source_balance * 100 / mean_balance);
    }

    action {
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("issuesimple"),
        issue_args {
            .org = org,
            .to = account,
            .badge = emit_badge,
            .amount = emit_amount,
            .memo = "issued from rollup consumer"
        }
    }.send();
}

ACTION giverrep::newemission(
    name org,
    name badge,
    name source_account_rep_badge,
    name emit_badge) {
    // Ensure the action is authorized by the contract itself
    require_auth(_self);

    // Create an instance of the emissions_table scoped by the organization
    emissions_table emissions(_self, org.value);

    // Check if an entry with the specified emission_name already exists
    auto existing_emission = emissions.find(badge.value);

    // Ensure that the emission does not already exist
    eosio::check(existing_emission == emissions.end(), "Emission with this name already exists");

    // Insert the new emission into the table with status set to "INIT"
    emissions.emplace(_self, [&](auto& new_emission) {
        new_emission.badge = badge;
        new_emission.source_account_rep_badge = source_account_rep_badge;
    });
}

ACTION giverrep::issuesimple(name org, name to, name badge, uint64_t amount, string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}
