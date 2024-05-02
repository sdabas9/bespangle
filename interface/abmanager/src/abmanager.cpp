#include <abmanager.hpp>

// Action to create an antibadge
ACTION abmanager::create(name authorized, 
                         name org, 
                         name anti_badge,
                         name badge,
                         name type,
                         string offchain_lookup_data,
                         string onchain_lookup_data,
                         vector<name> consumers,
                         string memo) {

    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(ANTIBADGE_VALIDATION_CONTRACT));

    string action_name = "create";
    string failure_identifier = "CONTRACT: abmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    // Validate and retrieve symbols
    symbol badge_symbol = validate_and_get_symbol(org, badge, failure_identifier);
    symbol anti_badge_symbol = validate_and_get_symbol(org, anti_badge, failure_identifier); 

    // Sending action to ANTIBADGE_CONTRACT
    action {
        permission_level{get_self(), name("active")},
        name(ANTIBADGE_CONTRACT),
        name("create"),
        create_args {
            .anti_badge_symbol = anti_badge_symbol,
            .badge_symbol = badge_symbol,
            .type = type,
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = onchain_lookup_data,
            .memo = memo }
    }.send();

    // Loop through consumers to add feature
    for (auto i = 0; i < consumers.size(); i++) {
        action {
            permission_level{get_self(), name("active")},
            name(ORCHESTRATOR_CONTRACT),
            name("addfeature"),
            addfeature_args {
                .badge_symbol = anti_badge_symbol,
                .notify_account = consumers[i],
                .memo = memo}
        }.send();
    } 
}

// Action to create an antibadge with inverse logic
ACTION abmanager::createinv(name authorized, 
                            name org, 
                            name anti_badge,
                            name badge,
                            string offchain_lookup_data,
                            string onchain_lookup_data,
                            vector<name> consumers,
                            string memo) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(ANTIBADGE_VALIDATION_CONTRACT));

    string action_name = "createinv";
    string failure_identifier = "CONTRACT: abmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    // Validate and retrieve symbols
    symbol badge_symbol = validate_and_get_symbol(org, badge, failure_identifier);
    symbol anti_badge_symbol = validate_and_get_symbol(org, anti_badge, failure_identifier); 

    // Sending action to ANTIBADGE_CONTRACT with inverse logic
    action {
        permission_level{get_self(), name("active")},
        name(ANTIBADGE_CONTRACT),
        name("createinv"),
        createinv_args {
            .anti_badge_symbol = anti_badge_symbol,
            .badge_symbol = badge_symbol,
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = onchain_lookup_data,
            .memo = memo }
    }.send();

    // Loop through consumers to add feature with inverse logic
    for (auto i = 0; i < consumers.size(); i++) {
        action {
            permission_level{get_self(), name("active")},
            name(ORCHESTRATOR_CONTRACT),
            name("addfeature"),
            addfeature_args {
                .badge_symbol = badge_symbol,
                .notify_account = consumers[i],
                .memo = memo}
        }.send();
    } 
}

// Action to issue an antibadge
ACTION abmanager::issue(name authorized, 
                        name org,
                        name anti_badge, 
                        uint64_t amount,
                        name to, 
                        string memo) {
    
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(ANTIBADGE_VALIDATION_CONTRACT));

    string action_name = "issue";
    string failure_identifier = "CONTRACT: abmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    // Validate and retrieve the anti_badge symbol
    symbol anti_badge_symbol = validate_and_get_symbol(org, anti_badge, failure_identifier);
    
    // Prepare the asset for the action
    asset anti_badge_asset(amount, anti_badge_symbol);

    // Sending action to ANTIBADGE_CONTRACT to issue the antibadge
    action {
        permission_level{get_self(), name("active")},
        name(ANTIBADGE_CONTRACT),
        name("issue"),
        issue_args {
            .anti_badge_asset = anti_badge_asset,
            .to = to,
            .memo = memo }
    }.send();
}
