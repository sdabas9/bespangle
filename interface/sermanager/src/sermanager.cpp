#include <sermanager.hpp>

  ACTION sermanager::naddfeatur (name org,
    name badge_name, 
    name notify_account, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }


  ACTION sermanager::createseries(name org, name authorized, name series) {

    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(SERIES_VALIDATION_CONTRACT));

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("ncreateserie"),
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
      name(get_self()),
      name("ncreatenex"),
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
        name(get_self()),
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
      name(get_self()),
      name("nissuelates"),
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
      name(get_self()),
      name("nissuean"),
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
        name(get_self()),
        name("nissuelates"),
        issuelatest_args {
          .org = org,
          .series = series,
          .to = to[i],
          .count = 1,
          .memo = memo }
      }.send();  
    }
  }


  ACTION sermanager::ncreateserie (name org, 
    name series) {
    require_auth(get_self());
    require_recipient(name(SERIES_NOTIFICATION_CONTRACT));    
  }

  ACTION sermanager::ncreatenex (name org,
    name series, 
    name badge, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(SERIES_NOTIFICATION_CONTRACT));
  }

  ACTION sermanager::nissuelates (name org,
    name series, 
    name to, 
    uint64_t count, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(SERIES_NOTIFICATION_CONTRACT));
  }

  ACTION sermanager::nissuean (name org,
    name series,
    uint64_t seq_id,
    name to,
    uint64_t count,
    string memo) {
    require_auth(get_self());
    require_recipient(name(SERIES_NOTIFICATION_CONTRACT));  
  }
