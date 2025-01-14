#include <mrmanager.hpp>

ACTION mrmanager::init(
    name authorized, 
    symbol badge_symbol, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string offchain_lookup_data,
    string onchain_lookup_data,
    bool lifetime_aggregate,
    bool lifetime_stats,
    string memo
) {
    require_auth(authorized);
    
    string action_name = "init";
    string failure_identifier = "CONTRACT: mrmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);
    
    if(org != authorized) {
        require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    }

    notify_checks_contract(org);
    
    action(
        permission_level{get_self(), name("active")},
        name(MUTUAL_RECOGNITION_CONTRACT),
        name("create"),
        create_args {
            .org = org,
            .badge_symbol = badge_symbol,
            .starttime = starttime,
            .cycle_length = cycle_length,
            .supply_per_cycle = supply_per_cycle,
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = onchain_lookup_data,
            .memo = memo
        }
    ).send();

    vector<name> consumers;
    consumers.push_back(name(SUBSCRIPTION_CONTRACT));
    if (lifetime_aggregate) {
        consumers.push_back(name(CUMULATIVE_CONTRACT));
    }
    if (lifetime_aggregate && lifetime_stats) {
        consumers.push_back(name(STATISTICS_CONTRACT));
    } else if (!lifetime_aggregate && lifetime_stats) {
        check(false, "Enable Lifetime aggregates to capture Lifetime stats");
    }
    
    for (auto& consumer : consumers) {
        action(
            permission_level{get_self(), name("active")},
            name(ORCHESTRATOR_CONTRACT),
            name("addfeature"),
            addfeature_args {
                .org = org,
                .badge_symbol = badge_symbol,
                .notify_account = consumer,
                .memo = memo
            }
        ).send();
    }
}

ACTION mrmanager::issue(
    symbol badge_symbol,
    uint64_t amount,
    name from, 
    name to,
    string memo
) {
    require_auth(from);
    require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));

    string action_name = "issue";
    string failure_identifier = "CONTRACT: mrmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    notify_checks_contract(org);

    action(
        permission_level{get_self(), name("active")},
        name(MUTUAL_RECOGNITION_CONTRACT),
        name("issue"),
        issue_args {
            .org = org,
            .badge_asset = eosio::asset(amount, badge_symbol),
            .from = from,
            .to = to,
            .memo = memo
        }
    ).send();
}

ACTION mrmanager::starttime(
    name authorized, 
    symbol badge_symbol, 
    time_point_sec new_starttime
) {
    require_auth(authorized);

    string action_name = "starttime";
    string failure_identifier = "CONTRACT: mrmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    if(org != authorized) {
        require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    }


    notify_checks_contract(org);

    action(
        permission_level{get_self(), name("active")},
        name(MUTUAL_RECOGNITION_CONTRACT),
        name("starttime"),
        starttime_args {
            .org = org,
            .badge_symbol = badge_symbol,
            .new_starttime = new_starttime
        }
    ).send();
}

ACTION mrmanager::cyclelength(
    name authorized, 
    symbol badge_symbol, 
    uint64_t new_cycle_length
) {
    require_auth(authorized);

    string action_name = "cyclelength";
    string failure_identifier = "CONTRACT: mrmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    if(org != authorized) {
        require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    }

    notify_checks_contract(org);

    action(
        permission_level{get_self(), name("active")},
        name(MUTUAL_RECOGNITION_CONTRACT),
        name("cyclelength"),
        cyclelength_args {
            .org = org,
            .badge_symbol = badge_symbol,
            .new_cycle_length = new_cycle_length
        }
    ).send();
}

ACTION mrmanager::cyclesupply(
    name authorized, 
    symbol badge_symbol, 
    uint8_t new_supply_per_cycle
) {
    require_auth(authorized);

    string action_name = "cyclesupply";
    string failure_identifier = "CONTRACT: mrmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    if(org != authorized) {
        require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    }

    notify_checks_contract(org);

    action(
        permission_level{get_self(), name("active")},
        name(MUTUAL_RECOGNITION_CONTRACT),
        name("cyclesupply"),
        cyclesupply_args {
            .org = org,
            .badge_symbol = badge_symbol,
            .new_supply_per_cycle = new_supply_per_cycle
        }
    ).send();
}
