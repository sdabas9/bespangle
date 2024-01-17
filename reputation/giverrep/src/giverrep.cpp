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
    if(keystats_itr == keystats.end()) {
        return;
    }
    //eosio::check(keystats_itr != keystats.end(), "Stats not setup for givers rep badge " + giver_rep_badge.to_string());
    uint64_t mean_balance = keystats_itr->total/keystats_itr->account_count;

    uint64_t source_balance = account_balance(org, from, giver_rep_badge);
    uint64_t emit_amount;
    emit_amount = source_balance * 100 / mean_balance;
    if (emit_amount == 0 && source_balance > 0) {
        emit_amount = 1;
    }
  /*  if ((source_balance * 100) % mean_balance == 0) {
        
         // 50 , 3, 6    
         // 34, 2, 6
         // 17 , 1, 6
         // 67, 4, 6
         // 84, 5 ,6
         // 100, 6, 6
         // 117, 7,6
         // 134, 8, 6


         // 100, 7, 7
         // 115, 8, 7
         // 129, 9, 7   
    } else {
        emit_amount = 1 + (source_balance * 100 / mean_balance);
    } */
    if(emit_amount > 0) {
        action {
            permission_level{get_self(), name("active")},
            name(SIMPLEBADGE_CONTRACT),
            name("issue"),
            issue_args {
                .org = org,
                .to = account,
                .badge = emit_badge,
                .amount = emit_amount,
                .memo = "issued from rollup consumer"
            }
        }.send();
    }

}

ACTION giverrep::newemission(
    name org,
    name badge,
    name source_account_rep_badge,
    name emit_badge) {
    // Ensure the action is authorized by the contract itself
    check_internal_auth(name("newemission"));

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
        new_emission.emit_badge = emit_badge;
    });
}

