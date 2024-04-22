#include <simmanager.hpp>

  ACTION simmanager::initsimple (name authorized, 
      name org, 
      name badge,
      string offchain_lookup_data, 
      string onchain_lookup_data,      
      bool lifetime_aggregate,
      bool lifetime_stats,
      bool emit_secondary_token,
      bool bounded_aggregate,
      bool bounded_stats,
      vector<name> other_consumers,
      string memo) {

    require_auth(authorized);
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
      .org = org,
      .badge_symbol = badge_symbol,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
    }.send();

    vector<name> consumers;
    if(lifetime_aggregate) {
      consumers.push_back(name(CUMULATIVE_CONTRACT));
    }
    if(lifetime_aggregate && lifetime_stats) {
      consumers.push_back(name(STATISTICS_CONTRACT));
    } else if (!lifetime_aggregate && lifetime_stats) {
      check(false, "Enable Lifetime aggregates to capture Lifetime stats");
    }
    if(emit_secondary_token) {
      consumers.push_back(name(ANDEMITTER_CONTRACT));
    }
    if(bounded_aggregate) {
      consumers.push_back(name(BOUNDED_AGG_CONTRACT));
    }
    if(bounded_aggregate && bounded_stats) {
      consumers.push_back(name(BOUNDED_STATS_CONTRACT));
    } else if (!bounded_aggregate && bounded_stats) {
      check(false, "Enable Bounded aggregates to capture Bounded stats");
    }
    consumers.insert(consumers.end(), other_consumers.begin(), other_consumers.end());
    
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

      if(consumers[i] == name(STATISTICS_CONTRACT)) {
        action {
          permission_level{get_self(), name("active")},
          name(STATISTICS_CONTRACT),
          name("settings"),
          settings_args {
            .org = org,
            .badge_symbol = badge_symbol,
            .max_no_of_ranks = 1000000000
          }
        }.send();
      }
    }
  }

  ACTION simmanager::simplebatch (name authorized,
      name org, 
      name badge,
      uint64_t amount,
      vector<name> to, 
      string memo) {
    require_auth(authorized);
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
        .org = org,
        .badge_asset = badge_asset,
        .to = to[i],
        .memo = memo }
      }.send();
    }
  }


  ACTION simmanager::givesimple (name authorized,
     name org,
     name badge,
     uint64_t amount, 
     name to, 
     string memo ) {
    require_auth(authorized);

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
      .org = org,
      .badge_asset = badge_asset,
      .to = to,
      .memo = memo }
    }.send();


  }

