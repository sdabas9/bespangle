#include <andemitter.hpp>

void andemitter::notifyachiev(name org, asset amount, name from, name to, string memo, vector<name> notify_accounts) {
    string action_name = "notifyachiev";
    string failure_identifier = "CONTRACT: andemitter, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 

    activelookup_table active_lookup(get_self(), get_self().value);
    auto lookup_itr = active_lookup.find(amount.symbol.code().raw());
    if(lookup_itr == active_lookup.end()) {
        return;
    }

    emissions_table emissions(get_self(), get_self().value);
    accounts_table accounts(get_self(), to.value);

    for (const auto& emission_name : lookup_itr->active_emissions) {
        auto emission_itr = emissions.find(emission_name.value);
        check(emission_itr != emissions.end(), "Emission does not exist");

        auto account_itr = accounts.find(emission_name.value);
        uint8_t emission_status = emission_itr->cyclic ? CYCLIC_IN_PROGRESS : NON_CYCLIC_IN_PROGRESS;

        if (account_itr != accounts.end() && account_itr->emission_status == NON_CYCLIC_EMITTED) continue;

        if (account_itr == accounts.end()) {
            account_itr = accounts.emplace(get_self(), [&](auto& acc) {
                acc.emission_name = emission_name;
                acc.emission_status = emission_status;
            });
        } else {
            // Modification logic commented out
            // accounts.modify(account_itr, same_payer, [&](auto& acc) {
            //     acc.expanded_emitter_status[amount.symbol.code()] += amount.amount;
            // });
        }
        auto expanded_emitter_status_map = account_itr->expanded_emitter_status;
        update_expanded_emitter_status(to, expanded_emitter_status_map, amount, *emission_itr, emission_status, failure_identifier);

        accounts.modify(account_itr, get_self(), [&](auto& acc) {
            acc.expanded_emitter_status = expanded_emitter_status_map;
            acc.emission_status = emission_status;
        });
    }
}

ACTION andemitter::newemission(name org, name emission_name, vector<asset> emitter_criteria_vector, vector<contract_asset> emit_assets, bool cyclic) {
    string action_name = "newemission";
    string failure_identifier = "CONTRACT: andemitter, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 
    validate_org_assets(org, emitter_criteria_vector, emit_assets, failure_identifier);

    emissions_table emissions(get_self(), get_self().value);
    auto emission_itr = emissions.find(emission_name.value);
    check(emission_itr == emissions.end(), "Emission already exists");

    map<symbol_code, asset> emitter_criteria_map;
    for (const auto& crit : emitter_criteria_vector) {
        emitter_criteria_map[crit.symbol.code()] = crit;
    }
    
    emissions.emplace(get_self(), [&](auto& em) {
        em.emission_name = emission_name;
        em.emitter_criteria = emitter_criteria_map;
        em.emit_assets = emit_assets;
        em.status = name("init");
        em.cyclic = cyclic;
    });
}

ACTION andemitter::chkorgasset(name org, vector<asset> emitter_criteria, vector<contract_asset> emit_assets) {
    require_auth(get_self());
    string action_name = "chkorgasset";
    string failure_identifier = "CONTRACT: andemitter, ACTION: " + action_name + ", MESSAGE: ";
    validate_org_assets(org, emitter_criteria, emit_assets, failure_identifier);
}

ACTION andemitter::activate(name org, name emission_name) {
    string action_name = "activate";
    string failure_identifier = "CONTRACT: andemitter, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 

    emissions_table emissions(get_self(), get_self().value);
    auto emission_itr = emissions.find(emission_name.value);
    check(emission_itr != emissions.end(), "Emission does not exist");
    vector<asset> asset_vector;

    // Iterate through the map and push each asset into the vector
    for (const auto& pair : emission_itr->emitter_criteria) {
        asset_vector.push_back(pair.second);
    }
    validate_org_assets(org, asset_vector, emission_itr->emit_assets, failure_identifier);

    emissions.modify(emission_itr, get_self(), [&](auto& mod) {
        mod.status = name("activate");
    });

    activelookup_table activelookup(get_self(), get_self().value);
    for (const auto& rec : emission_itr->emitter_criteria) {
        auto badge_symbol = rec.second.symbol;
        auto lookup_itr = activelookup.find(badge_symbol.code().raw());

        if (lookup_itr == activelookup.end()) {
            activelookup.emplace(get_self(), [&](auto& new_entry) {
                new_entry.badge_symbol = badge_symbol;
                new_entry.active_emissions.push_back(emission_name);
            });
        } else {
            activelookup.modify(lookup_itr, get_self(), [&](auto& mod_entry) {
                mod_entry.active_emissions.push_back(emission_name);
            });
        }
    }
}

ACTION andemitter::deactivate(name org, name emission_name) {
    string action_name = "deactivate";
    string failure_identifier = "CONTRACT: andemitter, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 

    emissions_table emissions(get_self(), get_self().value);
    auto emission_itr = emissions.find(emission_name.value);
    check(emission_itr != emissions.end(), "Emission does not exist");
    vector<asset> asset_vector;

    // Iterate through the map and push each asset into the vector
    for (const auto& pair : emission_itr->emitter_criteria) {
        asset_vector.push_back(pair.second);
    }

    validate_org_assets(org, asset_vector, emission_itr->emit_assets, failure_identifier);

    emissions.modify(emission_itr, get_self(), [&](auto& mod) {
        mod.status = name("deactivate");
    });

    activelookup_table activelookup(get_self(), get_self().value);
    for (const auto& rec : emission_itr->emitter_criteria) {
        auto badge_symbol = rec.second.symbol;
        auto lookup_itr = activelookup.find(badge_symbol.code().raw());

        if (lookup_itr != activelookup.end()) {
            auto& active_emissions = lookup_itr->active_emissions;
            auto it = std::find(active_emissions.begin(), active_emissions.end(), emission_name);

            if (it != active_emissions.end()) {
                activelookup.modify(lookup_itr, get_self(), [&](auto& mod) {
                    mod.active_emissions.erase(it);
                });

                if (lookup_itr->active_emissions.empty()) {
                    activelookup.erase(lookup_itr);
                }
            }
        }
    }
}

void andemitter::check_internal_auth(name action, string failure_identifier) {
    auth_table _auth(name(AUTHORITY_CONTRACT), _self.value);
    auto itr = _auth.find(action.value);
    check(itr != _auth.end(), failure_identifier + " - no entry in authority table for this action and contract");
    auto authorized_contracts = itr->authorized_contracts;
    for (auto& auth_contract : authorized_contracts) {
        if (has_auth(auth_contract)) {
            return;
        }
    }
    check(false, failure_identifier + " - Calling contract not in authorized list of accounts for action " + action.to_string());
}

void andemitter::invoke_action(name to, vector<contract_asset> emit_assets, uint8_t emit_factor, string failure_identifier) {
    for (const auto& rec : emit_assets) {
        int64_t new_amount = rec.emit_asset.amount * emit_factor;
        asset badge_asset = asset(new_amount, rec.emit_asset.symbol);
        name destination_org = get_org_from_badge_symbol(badge_asset.symbol, failure_identifier);

        if (rec.contract == name(SIMPLEBADGE_CONTRACT)) {
            action(
                permission_level{get_self(), "active"_n},
                name(SIMPLEBADGE_CONTRACT),
                "issue"_n,
                issue_args {
                    .org = destination_org,
                    .badge_asset = badge_asset,
                    .to = to,
                    .memo = "issued from andemitter" }
            ).send();
        }
        // Extend with more conditions if other contracts are to be called
    }
}

void andemitter::update_expanded_emitter_status(name account, map<symbol_code, int64_t>& expanded_emitter_status, const asset& new_asset, emissions emission, uint8_t& emission_status, string failure_identifier) {
    expanded_emitter_status[new_asset.symbol.code()] += new_asset.amount;

    bool criteria_met = true;
    int min_multiplier = INT_MAX;
    for (const auto& [symbol, crit_asset] : emission.emitter_criteria) {
        auto it = expanded_emitter_status.find(symbol);
        if (it == expanded_emitter_status.end() || it->second < crit_asset.amount) {
            criteria_met = false;
            break;
        } else {
            int multiplier = it->second / crit_asset.amount;
            min_multiplier = std::min(min_multiplier, multiplier);
        }
    }

    if (criteria_met) {
        if (!emission.cyclic && min_multiplier > 1) {
            min_multiplier = 1; // Adjust for non-cyclic emissions
        }

        if (!emission.cyclic) {
            emission_status = (min_multiplier > 0) ? NON_CYCLIC_EMITTED : NON_CYCLIC_IN_PROGRESS;
        } else {
            emission_status = CYCLIC_IN_PROGRESS;
        }

        if (min_multiplier > 0) {
            for (auto& [symbol, amount] : expanded_emitter_status) {
                amount -= emission.emitter_criteria[symbol].amount * min_multiplier;
                if (amount < 0) amount = 0; // Prevent negative values
            }
            invoke_action(account, emission.emit_assets, min_multiplier, failure_identifier); // Take action based on the new status
        }
    }
}

void andemitter::validate_org_assets(name org, const vector<asset>& emitter_criteria, const vector<contract_asset>& emit_assets, string failure_identifier) {

    // Validate emitter_criteria assets belong to org
    for (const auto& rec : emitter_criteria) {
        validate_org_badge_symbol(org, rec.symbol, failure_identifier);
    }
    
    // Validate emit_assets belong to org
    for (const auto& contract_asset : emit_assets) {
        validate_org_badge_symbol(org, contract_asset.emit_asset.symbol, failure_identifier);
    }

}
