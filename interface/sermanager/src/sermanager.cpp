#include <sermanager.hpp>

  ACTION sermanager::createseries(name org, name authorized, name series) {

    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(SERIES_VALIDATION_CONTRACT));

    action {
      permission_level{get_self(), name("active")},
      name(SERIESBADGE_CONTRACT),
      name("createseries"),
      createseries_args {
        .org = org,
        .series = series }
    }.send();  
  }

  ACTION sermanager::createnext(name org, name authorized, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, vector<name> consumers, string memo) {
    
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(SERIES_VALIDATION_CONTRACT));
    
    action {
      permission_level{get_self(), name("active")},
      name(SERIESBADGE_CONTRACT),
      name("createnext"),
      createnext_args {
        .org = org,
        .series = series,
        .badge = badge,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = onchain_lookup_data,
        .memo = memo }
    }.send();

    for (auto i = 0 ; i < consumers.size(); i++) {
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("naddfeatur"),
        addfeature_args {
          .org = org,
          .badge_name = badge,
          .notify_account = consumers[i],
          .memo = memo}
      }.send();
    }  
  }

  ACTION sermanager::issuelatest(name org, name authorized, name series, name to, uint64_t count, string memo) {
    
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(SERIES_VALIDATION_CONTRACT));
   
    action {
      permission_level{get_self(), name("active")},
      name(SERIESBADGE_CONTRACT),
      name("issuelatest"),
      issuelatest_args {
        .org = org,
        .series = series,
        .to = to,
        .count = count,
        .memo = memo }
    }.send();    
  }

  ACTION sermanager::issueany(name org, name authorized, name series, uint64_t seq_id, name to, uint64_t count, string memo) {

    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(SERIES_VALIDATION_CONTRACT));

    action {
      permission_level{get_self(), name("active")},
      name(SERIESBADGE_CONTRACT),
      name("issueany"),
      issueany_args {
        .org = org,
        .series = series,
        .seq_id = seq_id,
        .to = to,
        .count = count,
        .memo = memo }
    }.send();      
  }

  ACTION sermanager::serieslbatch (name org, name authorized, name series, vector<name> to, string memo) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(SERIES_VALIDATION_CONTRACT));

    for( auto i = 0; i < to.size(); i++ ) {
      action {
        permission_level{get_self(), name("active")},
        name(SERIESBADGE_CONTRACT),
        name("issuelatest"),
        issuelatest_args {
          .org = org,
          .series = series,
          .to = to[i],
          .count = 1,
          .memo = memo }
      }.send();  
    }
  }

