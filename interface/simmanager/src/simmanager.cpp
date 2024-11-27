#include <simmanager.hpp>

  ACTION simmanager::initsimple (name authorized, 
      symbol badge_symbol,
      string offchain_lookup_data, 
      string onchain_lookup_data,
      bool lifetime_aggregate,
      bool lifetime_stats,
      string memo) {

    require_auth(authorized);
    
    string action_name = "initsimple";
    string failure_identifier = "CONTRACT: simmanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    if(org != authorized) {
      require_recipient(name(SIMPLE_VALIDATION_CONTRACT));    
    }    

    notify_checks_contract(org);
    
    action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("create"),
    createsimple_args {
      .org = org,
      .badge_symbol = badge_symbol,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
    }.send();

    vector<name> consumers;
    consumers.push_back(name(SUBSCRIPTION_CONTRACT));
    if(lifetime_aggregate) {
      consumers.push_back(name(CUMULATIVE_CONTRACT));
    }
    if(lifetime_aggregate && lifetime_stats) {
      consumers.push_back(name(STATISTICS_CONTRACT));
    } else if (!lifetime_aggregate && lifetime_stats) {
      check(false, "Enable Lifetime aggregates to capture Lifetime stats");
    }
    
    for (auto i = 0 ; i < consumers.size(); i++) {
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args {
          .org = org,
          .badge_symbol = badge_symbol,
          .notify_account = consumers[i],
          .memo = memo}
      }.send();
    }
  }

  ACTION simmanager::simplebatch (name authorized,
      symbol badge_symbol,
      uint64_t amount,
      vector<name> to, 
      string memo) {
    require_auth(authorized);
    
    string action_name = "simplebatch";
    string failure_identifier = "CONTRACT: simmanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);
    
    if(org != authorized) {
      require_recipient(name(SIMPLE_VALIDATION_CONTRACT));    
    }    

    notify_checks_contract(org);
    asset badge_asset(amount, badge_symbol);
    
    for( auto i = 0; i < to.size(); i++ ) {
      action {
      permission_level{get_self(), name("active")},
      name(SIMPLEBADGE_CONTRACT),
      name("issue"),
      issuesimple_args {
        .org = org,
        .badge_asset = badge_asset,
        .to = to[i],
        .memo = memo }
      }.send();
    }
  }


  ACTION simmanager::givesimple (name authorized,
      symbol badge_symbol,
      uint64_t amount, 
      name to, 
      string memo ) {
    require_auth(authorized);
    string action_name = "givesimple";
    string failure_identifier = "CONTRACT: simmanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    if(org != authorized) {
      require_recipient(name(SIMPLE_VALIDATION_CONTRACT));    
    }    

    notify_checks_contract(org);
    asset badge_asset(amount, badge_symbol);

    action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("issue"),
    issuesimple_args {
      .org = org,
      .badge_asset = badge_asset,
      .to = to,
      .memo = memo }
    }.send();


  }

