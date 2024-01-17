#include <simmanager.hpp>

  ACTION simmanager::initsimple (name org,
    name creator, 
    name badge, 
    vector<name> parent_badges,
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    vector<name> consumers,
    string memo) {
    require_auth(creator);

    notify_checks_contract(org);
    require_recipient(name(SIMPLE_VALIDATION_CONTRACT));

    action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("create"),
    createsimple_args {
      .org = org,
      .badge = badge,
      .parent_badges = parent_badges,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo}
    }.send();

    for (auto i = 0 ; i < consumers.size(); i++) {
      action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      addfeature_args {
        .org = org,
        .badge_name = badge,
        .notify_account = consumers[i],
        .memo = memo}
      }.send();
    }
  }

  ACTION simmanager::simplebatch (name org, name badge, name authorizer, vector<name> to, string memo) {
    require_auth(authorizer);
    notify_checks_contract(org);
    require_recipient(name(SIMPLE_VALIDATION_CONTRACT));

    for( auto i = 0; i < to.size(); i++ ) {
      action {
      permission_level{get_self(), name("active")},
      name(SIMPLEBADGE_CONTRACT),
      name("issue"),
      issuesimple_args {
        .org = org,
        .to = to[i],
        .badge = badge,
        .amount = 1,
        .memo = memo }
      }.send();
    }
  }


  ACTION simmanager::givesimple (name org, name badge, name authorizer, name to, string memo ) {
    require_auth(authorizer);

    notify_checks_contract(org);
    require_recipient(name(SIMPLE_VALIDATION_CONTRACT));

    action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("issue"),
    issuesimple_args {
      .org = org,
      .to = to,
      .badge = badge,
      .amount = 1,
      .memo = memo }
    }.send();


  }

