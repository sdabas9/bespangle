#include <giverrep.hpp>


void giverrep::notifyachiev (
      name org,
      asset badge_asset, 
      name from, 
      name to, 
      string memo, 
      vector<name> notify_accounts
    ) {
    string action_name = "notifyachiev";
    string failure_identifier = "CONTRACT: giverrep, ACTION: " + action_name + ", MESSAGE: ";
    
    emissions_table emissions(_self, _self.value);
    auto existing_emission = emissions.find(badge_asset.symbol.code().raw());

    // Ensure that the emission does not already exist
    eosio::check(existing_emission != emissions.end(), "Giver rep badge not setup");

    if(existing_emission->status != name("active")) {
        return;
    }
    
    symbol giver_rep_badge_symbol = existing_emission->giver_rep_badge_symbol;
    vector<symbol> emit_badge_symbols = existing_emission->emit_badge_symbols;

    counts_table counts(name(STATISTICS_CONTRACT), org.value);
    auto counts_itr = counts.find(giver_rep_badge_symbol.code().raw());
    if(counts_itr == counts.end()) {
        return;
    }
    //eosio::check(counts_itr != counts.end(), "Stats not setup for givers rep badge " + giver_rep_badge.to_string());
    uint64_t mean_balance = counts_itr->total_issued/counts_itr->total_recipients;
    accounts _accounts(name(CUMULATIVE_CONTRACT), from.value);
    auto accounts_itr = _accounts.find(giver_rep_badge_symbol.code().raw());

    uint64_t source_balance = (accounts_itr == _accounts.end()) ? 0 : accounts_itr->balance.amount;
    uint64_t emit_amount;
    emit_amount = source_balance * 100 / mean_balance;
    if (emit_amount == 0 && source_balance > 0) {
        emit_amount = 1;
    }
    if(emit_amount > 0) {
        for( auto i = 0; i < emit_badge_symbols.size(); i++ ) {
            name issuing_org = get_org_from_badge_symbol(emit_badge_symbols[i], failure_identifier);
            asset emit_asset = eosio::asset(emit_amount, emit_badge_symbols[i]);
            action(
                permission_level{get_self(), "active"_n},
                name(SIMPLEBADGE_CONTRACT),
                "issue"_n,
                issue_args {
                    .org = issuing_org,
                    .badge_asset = emit_asset,
                    .to = to,
                    .memo = "issued from giverrep" }
            ).send();
        }
    }

}

ACTION giverrep::newemission(
      name org,  
      symbol badge_symbol,
      symbol giver_rep_badge_symbol, 
      vector<symbol> emit_badge_symbols) {

    string action_name = "newemission";
    string failure_identifier = "CONTRACT: giverrep, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    validate_org_badge_symbol(org, badge_symbol, failure_identifier);
    validate_org_badge_symbol(org, giver_rep_badge_symbol, failure_identifier);
    for(auto i = 0 ; i < emit_badge_symbols.size(); i++) {
        validate_org_badge_symbol(org, emit_badge_symbols[i], failure_identifier);
    }
    
    // Create an instance of the emissions_table scoped by the organization
    emissions_table emissions(_self, _self.value);

    // Check if an entry with the specified emission_name already exists
    auto existing_emission = emissions.find(badge_symbol.code().raw());

    // Ensure that the emission does not already exist
    eosio::check(existing_emission == emissions.end(), "Emission with this name already exists");

    // Insert the new emission into the table with status set to "INIT"
    emissions.emplace(_self, [&](auto& new_emission) {
        new_emission.badge_symbol = badge_symbol;
        new_emission.giver_rep_badge_symbol = giver_rep_badge_symbol;
        new_emission.emit_badge_symbols = emit_badge_symbols;
        new_emission.status = name("init");
    });
}

ACTION giverrep::activate(name org, symbol badge_symbol) {
    string action_name = "activate";
    string failure_identifier = "CONTRACT: giverrep, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 

    emissions_table emissions(get_self(), get_self().value);
    auto emission_itr = emissions.find(badge_symbol.code().raw());
    check(emission_itr != emissions.end(), "Emission does not exist");
    
    validate_org_badge_symbol(org, emission_itr->badge_symbol, failure_identifier);
    validate_org_badge_symbol(org, emission_itr->giver_rep_badge_symbol, failure_identifier);
    for(auto i = 0 ; i < emission_itr->emit_badge_symbols.size(); i++) {
        validate_org_badge_symbol(org, emission_itr->emit_badge_symbols[i], failure_identifier);
    }

    emissions.modify(emission_itr, get_self(), [&](auto& mod) {
        mod.status = name("active");
    });

}

ACTION giverrep::deactivate(name org, symbol badge_symbol) {
    string action_name = "deactivate";
    string failure_identifier = "CONTRACT: giverrep, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 

    emissions_table emissions(get_self(), get_self().value);
    auto emission_itr = emissions.find(badge_symbol.code().raw());
    check(emission_itr != emissions.end(), "Emission does not exist");
    
    validate_org_badge_symbol(org, emission_itr->badge_symbol, failure_identifier);
    validate_org_badge_symbol(org, emission_itr->giver_rep_badge_symbol, failure_identifier);
    for(auto i = 0 ; i < emission_itr->emit_badge_symbols.size(); i++) {
        validate_org_badge_symbol(org, emission_itr->emit_badge_symbols[i], failure_identifier);
    }

    emissions.modify(emission_itr, get_self(), [&](auto& mod) {
        mod.status = name("inactive");
    });
}

