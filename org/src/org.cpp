#include <org.hpp>

// todo
// 1) remove check for init missing in settings for checks contract.

  ACTION org::chkscontract (name org, name checks_contract) {
    require_auth(org);
    checks_table _checks( get_self(), get_self().value );
    auto itr = _checks.find(org.value);
    if(itr == _checks.end()) {
      _checks.emplace(org, [&](auto& row) {
        row.org = org;
        row.checks_contract = checks_contract;
      });
    } else {
      _checks.modify(itr, org,[&](auto& row) {
        row.checks_contract = checks_contract;
      });
    }
  }

  ACTION org::processsync (name org, name action, name authorizer) {
    require_auth(authorizer); 
    require_recipient(checkscontract(org));
    processmode_table _processmode( get_self(), org.value);
    auto itr = _processmode.find(action.value);
    if( itr == _processmode.end()) {
      _processmode.emplace(org, [&](auto& row) {
        row.action = action;
        row.async = false;
      });      
    } else {
      _processmode.modify(itr, org,[&](auto& row) {
        row.async = false;
      });    
    }
  }

  ACTION org::processasync (name org, name action, name authorizer) {
    require_auth(authorizer); 
    require_recipient(checkscontract(org));
    processmode_table _processmode( get_self(), org.value);
    auto itr = _processmode.find(action.value);
    if( itr == _processmode.end()) {
      _processmode.emplace(org, [&](auto& row) {
        row.action = action;
        row.async = true;
      });      
    } else {
      _processmode.modify(itr, org,[&](auto& row) {
        row.async = true;
      });    
    }
  }

  ACTION org::initsimple (name org,
    name creator, 
    name badge, 
    vector<name> parent_badges,
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    vector<name> consumers,
    string memo) {
    require_auth(creator);

    require_recipient(checkscontract(org));


    action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("ninitsimpl"),
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
      name(get_self()),
      name("naddfeatur"),
      orchestrator_addfeature_args {
        .org = org,
        .badge_name = badge,
        .notify_account = consumers[i],
        .memo = memo}
      }.send();
    }
  }

  ACTION org::ninitsimpl (name org,
    name badge, 
    vector<name> parent_badges,
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));

  }

  ACTION org::naddfeatur (name org,
    name badge_name, 
    name notify_account, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }

  ACTION org::initgotcha (name org,
    name creator, 
    name badge, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string offchain_lookup_data, 
    string onchain_lookup_data,
    vector<name> consumers,
    string memo) {

    require_auth(creator);
    
    require_recipient(checkscontract(org));
    
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("ninitgotch"),
      creategotcha_args {
        .org = org,
        .badge = badge,
        .starttime = starttime,
        .cycle_length = cycle_length,
        .supply_per_cycle = supply_per_cycle,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = onchain_lookup_data,
        .memo = memo }
    }.send();

    for (auto i = 0 ; i < consumers.size(); i++) {
      action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("naddfeatur"),
      orchestrator_addfeature_args {
        .org = get_self(),
        .badge_name = badge,
        .notify_account = consumers[i],
        .memo = memo}
      }.send();
    }
  }

  ACTION org::ninitgotch (name org, 
    name badge, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string offchain_lookup_data, 
    string onchain_lookup_data,
    vector<name> consumers,
    string memo) {

    require_auth(get_self());
    require_recipient(name(GOTCHABADGE_CONTRACT));

  }

  ACTION org::createseries(name org, name authorized, name series) {

    require_auth(authorized);
    require_recipient(checkscontract(org));

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("ncreateserie"),
      createseries_args {
        .org = org,
        .series = series }
    }.send();  
  }

  ACTION org::createnext(name org, name authorized, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, vector<name> consumers, string memo) {
    
    require_auth(authorized);
    require_recipient(checkscontract(org));
    
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
        orchestrator_addfeature_args {
          .org = org,
          .badge_name = badge,
          .notify_account = consumers[i],
          .memo = memo}
      }.send();
    }  
  }

  ACTION org::issuelatest(name org, name authorized, name series, name to, uint64_t count, string memo) {
    
    require_auth(authorized);
    require_recipient(checkscontract(org));
    
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

  ACTION org::issueany(name org, name authorized, name series, uint64_t seq_id, name to, uint64_t count, string memo) {

    require_auth(authorized);
    require_recipient(checkscontract(org));

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

  ACTION org::serieslbatch (name org, name authorized, name series, vector<name> to, string memo) {
    require_auth(authorized);
    require_recipient(checkscontract(org));

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


  ACTION org::ncreateserie (name org, 
    name series) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));    
  }

  ACTION org::ncreatenex (name org,
    name series, 
    name badge, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }

  ACTION org::nissuelates (name org,
    name series, 
    name to, 
    uint64_t count, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }

  ACTION org::nissuean (name org,
    name series,
    uint64_t seq_id,
    name to,
    uint64_t count,
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));  
  }

  ACTION org::givegotcha (name org, name badge, name from, name to, uint8_t amount, string memo ) {
    require_auth(from);
    require_recipient(checkscontract(org));

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("ngivegotch"),
      givegotcha_args {
        .org = org,
        .badge = badge,
        .from = from,
        .to = to,
        .amount = amount,
        .memo = memo }
    }.send();

  }

  ACTION org::ngivegotch (name org, name badge, name from, name to, uint8_t amount, string memo) {
    require_auth(get_self());
    require_recipient(name(GOTCHABADGE_CONTRACT));
  }

  ACTION org::simplebatch (name org, name badge, name authorizer, vector<name> to, string memo) {
    require_auth(authorizer);
    require_recipient(checkscontract(org));

    for( auto i = 0; i < to.size(); i++ ) {
      action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("ngivesimpl"),
      issuesimple_args {
        .org = org,
        .to = to[i],
        .badge = badge,
        .amount = 1,
        .memo = memo }
      }.send();
    }
  }


  ACTION org::givesimple (name org, name badge, name authorizer, name to, string memo ) {
    require_auth(authorizer);

    require_recipient(checkscontract(org));

    if(is_async(org, name("givesimple"))) {
      action {
      permission_level{get_self(), name("active")},
      name(ASYNC_CONTRACT),
      name("qgivesimpl"),
      qgivesimpl_args {
        .org = org,
        .to = to,
        .badge = badge,
        .memo = memo }
      }.send();

    } else {
      action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("ngivesimpl"),
      issuesimple_args {
        .org = org,
        .to = to,
        .badge = badge,
        .amount = 1,
        .memo = memo }
      }.send();
    }

  }

  ACTION org::agivesimpl(name org, name to, name badge, string memo ) {
    require_auth(name(ASYNC_CONTRACT));
    action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("ngivesimpl"),
    issuesimple_args {
      .org = org,
      .to = to,
      .badge = badge,
      .amount = 1,
      .memo = memo }
    }.send();
  }

  ACTION org::ngivesimpl(name org, name to, name badge, uint8_t amount, string memo ) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }


  ACTION org::initcasset(name org, 
    name creator,
    name assetname,
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    vector<name> consumers,
    string memo) {
    
    require_auth(creator);

    require_recipient(checkscontract(org));


    action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("ninitcasse"),
    create_claim_asset_args {
      .org = org,
      .assetname = assetname,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo}
    }.send();

    for (auto i = 0 ; i < consumers.size(); i++) {
      action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("naddfeatur"),
      orchestrator_addfeature_args {
        .org = org,
        .badge_name = assetname,
        .notify_account = consumers[i],
        .memo = memo}
      }.send();
    }
  }

  ACTION org::addclaimer(name org, 
    name authorizer, 
    name account, 
    name assetname, 
    uint64_t account_cap, 
    string memo) {
    require_auth(authorizer);

    require_recipient(checkscontract(org));

    action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("naddclaime"),
    add_claimer_args {
      .org = org,
      .account = account,
      .assetname = assetname,
      .account_cap = account_cap,
      .memo = memo}
    }.send();

  }

  ACTION org::claimasset(name org, 
    name to, 
    name assetname, 
    string memo ) {

    require_auth(to);

    require_recipient(checkscontract(org));
    
    action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("nclaimasse"),
    claim_asset_args {
      .org = org,
      .to = to,
      .assetname = assetname,
      .memo = memo}
    }.send();
  }

  ACTION org::ninitcasse(name org, 
    name assetname,
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }

  ACTION org::naddclaime(name org, 
    name account, 
    name assetname, 
    uint64_t account_cap, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }

  ACTION org::nclaimasse(name org, 
    name to, 
    name assetname, 
    string memo ) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }


