#include <orchmanager.hpp>

ACTION orchmanager::delfeature (name org,
    name authorized, 
    name badge,
    name consumer,
    string memo) {

    require_auth(authorized);
    notify_checks_contract(org);

    if(org!=authorized) {
      require_recipient(name(ORCHESTRATOR_VALIDATION_CONTRACT));    
    }

    string action_name = "delfeature";
    string failure_identifier = "CONTRACT: orchmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol badge_symbol = validate_and_get_symbol(org, badge, failure_identifier);
    
      action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("delfeature"),
      delfeature_args {
        .badge_symbol = badge_symbol,
        .notify_account = consumer,
        .memo = memo}
      }.send();
}

