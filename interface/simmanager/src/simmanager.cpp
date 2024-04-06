#include <simmanager.hpp>

  ACTION simmanager::initsimple (name org,
    name creator, 
    name badge,
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    vector<name> consumers,
    string memo) {

    require_auth(creator);
    notify_checks_contract(org);
    require_recipient(name(SIMPLE_VALIDATION_CONTRACT));

    string action_name = "initsimple";
    string failure_identifier = "CONTRACT: simmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol badge_symbol = validate_and_get_symbol(org, badge, failure_identifier);
    
    action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("create"),
    createsimple_args {
      .badge_symbol = badge_symbol,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
    }.send();

    for (auto i = 0 ; i < consumers.size(); i++) {
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

  ACTION simmanager::simplebatch (name org, name badge, uint64_t amount, name authorizer, vector<name> to, string memo) {
    require_auth(authorizer);
    notify_checks_contract(org);
    require_recipient(name(SIMPLE_VALIDATION_CONTRACT));

    string action_name = "simplebatch";
    string failure_identifier = "CONTRACT: simmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol badge_symbol = validate_and_get_symbol(org, badge, failure_identifier);
    asset badge_asset(amount, badge_symbol);
    
    for( auto i = 0; i < to.size(); i++ ) {
      action {
      permission_level{get_self(), name("active")},
      name(SIMPLEBADGE_CONTRACT),
      name("issue"),
      issuesimple_args {
        .badge_asset = badge_asset,
        .to = to[i],
        .memo = memo }
      }.send();
    }
  }


  ACTION simmanager::givesimple (name org, name badge, uint64_t amount, name authorizer, name to, string memo ) {
    require_auth(authorizer);

    notify_checks_contract(org);
    require_recipient(name(SIMPLE_VALIDATION_CONTRACT));

    string action_name = "givesimple";
    string failure_identifier = "CONTRACT: simmanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol badge_symbol = validate_and_get_symbol(org, badge, failure_identifier);
    asset badge_asset(amount, badge_symbol);

    action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("issue"),
    issuesimple_args {
      .badge_asset = badge_asset,
      .to = to,
      .memo = memo }
    }.send();


  }

