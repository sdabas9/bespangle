#include <hllemitter.hpp>

void hllemitter::notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts
) {
    string action_name = "notifyachiev";
    string failure_identifier = "CONTRACT: hllemitter, ACTION: " + action_name + ", MESSAGE: ";
    // Create an instance of the emissions_table scoped by the organization
    emissions_table emissions(_self, _self.value);

    // Check if an entry with the specified badge  exists
    auto existing_emission = emissions.find(badge_asset.symbol.code().raw());
    vector<asset> emit_assets ;

    check(existing_emission != emissions.end(), failure_identifier + "Uniqueness badge not defined for a badge with hll consumer");
    if(existing_emission->status != name("active")) {
        return;
    }
    emit_assets = existing_emission->sender_uniqueness_badge_assets;
    
    // Create an instance of the balances_table scoped by the organization
    balances_table balances(_self, to.value);

    auto itr = balances.find(badge_asset.symbol.code().raw());
    uint8_t b = 7;
    uint32_t m = 1 << b;
    if (itr == balances.end()) {
        // Initialize HyperLogLog data structure
        vector<uint8_t> M(m, 0);
        hll::HyperLogLog hll(b, m, M);
        hll.add(from.to_string().c_str(), from.to_string().size());

        // No existing entry found, insert a new record
        balances.emplace(_self, [&](auto& row) {
            row.badge_symbol = badge_asset.symbol;
            row.hll = hll.registers();
        });
        for(auto i = 0; i < emit_assets.size(); i++) {
            name destination_org = get_org_from_badge_symbol(emit_assets[i].symbol, failure_identifier);
            action(
                permission_level{get_self(), "active"_n},
                name(SIMPLEBADGE_CONTRACT),
                "issue"_n,
                issue_args {
                    .org = destination_org,
                    .badge_asset = emit_assets[i],
                    .to = to,
                    .memo = "issued from andemitter" }
            ).send();
        }

    } else {
        bool increment = false;
        
        // Get existing HyperLogLog data
        vector<uint8_t> M = itr->hll;
        hll::HyperLogLog hll(b, m, M);
        
        // Estimate cardinality before and after adding new element
        double estimate_before = hll.estimate();
        hll.add(from.to_string().c_str(), from.to_string().size());
        double estimate_after = hll.estimate();

        // Check if cardinality has increased
        if (estimate_before != estimate_after) {
            balances.modify(itr, get_self(), [&](auto& row) {
                row.hll = hll.registers();
            });
            for(auto i = 0; i < emit_assets.size(); i++) {
                name destination_org = get_org_from_badge_symbol(emit_assets[i].symbol, failure_identifier);
                action(
                    permission_level{get_self(), "active"_n},
                    name(SIMPLEBADGE_CONTRACT),
                    "issue"_n,
                    issue_args {
                        .org = destination_org,
                        .badge_asset = emit_assets[i],
                        .to = to,
                        .memo = "issued from andemitter" }
                ).send();
            }
        }
    }
}


ACTION hllemitter::newemission(
    name org,
    symbol badge_symbol,
    vector<symbol> sender_uniqueness_badge_symbols
) {
    string action_name = "newemission";
    string failure_identifier = "CONTRACT: hllemitter, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name("newemission"), failure_identifier);
    vector<asset> sender_uniqueness_badge_assets; 
    for (auto i = 0; i < sender_uniqueness_badge_symbols.size(); i++) {
        check(badge_symbol != sender_uniqueness_badge_symbols[i], "badge and sender_uniqueness_badge can not be same");
        sender_uniqueness_badge_assets.push_back(asset(1, sender_uniqueness_badge_symbols[i]));
    }
    validate_org_badge_symbol(org, badge_symbol, failure_identifier);
    for(auto i = 0; i < sender_uniqueness_badge_symbols.size(); i++) {
        validate_org_badge_symbol(org, sender_uniqueness_badge_symbols[i], failure_identifier);
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
        new_emission.sender_uniqueness_badge_assets = sender_uniqueness_badge_assets;
        new_emission.status = name("init");
    });

    action {
        permission_level{get_self(), name("active")},
        name(SUBSCRIPTION_CONTRACT),
        name("billing"),
        billing_args {
            .org = org,
            .actions_used = 1}
    }.send();
}

ACTION hllemitter::activate(name org, symbol badge_symbol) {
    string action_name = "activate";
    string failure_identifier = "CONTRACT: hllemitter, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 

    emissions_table emissions(get_self(), get_self().value);
    auto emission_itr = emissions.find(badge_symbol.code().raw());
    check(emission_itr != emissions.end(), failure_identifier + "Emission does not exist");
    
    validate_org_badge_symbol(org, emission_itr->badge_symbol, failure_identifier);
    for(auto i = 0 ; i < emission_itr->sender_uniqueness_badge_assets.size(); i++) {
        validate_org_badge_symbol(org, emission_itr->sender_uniqueness_badge_assets[i].symbol, failure_identifier);
    }

    emissions.modify(emission_itr, get_self(), [&](auto& mod) {
        mod.status = name("active");
    });

}

ACTION hllemitter::deactivate(name org, symbol badge_symbol) {
    string action_name = "deactivate";
    string failure_identifier = "CONTRACT: hllemitter, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 

    emissions_table emissions(get_self(), get_self().value);
    auto emission_itr = emissions.find(badge_symbol.code().raw());
    check(emission_itr != emissions.end(), "Emission does not exist");
    
    validate_org_badge_symbol(org, emission_itr->badge_symbol, failure_identifier);
    for(auto i = 0 ; i < emission_itr->sender_uniqueness_badge_assets.size(); i++) {
        validate_org_badge_symbol(org, emission_itr->sender_uniqueness_badge_assets[i].symbol, failure_identifier);
    }

    emissions.modify(emission_itr, get_self(), [&](auto& mod) {
        mod.status = name("inactive");
    });
}








