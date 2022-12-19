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
        .badge_contract = name(SIMPLEBADGE_CONTRACT),
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
    require_recipient(name(SIMPLEBADGE_CONTRACT));

  }

  ACTION org::naddfeatur (name org, 
    name badge_contract, 
    name badge_name, 
    name notify_account, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(ORCHESTRATOR_CONTRACT));
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
        .badge_contract = name(GOTCHABADGE_CONTRACT),
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

  ACTION org::defineseries (name org, name creator, name family) {
    require_auth(creator);
    require_recipient(checkscontract(org));

    action {
      permission_level{get_self(), name("active")},
      name("seriesbadge"),
      name("define"),
      defineseries_args {
        .org = get_self(),
        .family = family
      }
    }.send();
  }

  ACTION org::initseriesbdg (name org, name creator, 
    name family, 
    name badge, 
    string offchain_lookup_data, 
    string onchain_lookup_data,
    vector<name> consumers,
    string memo) {

    require_auth(creator);
    require_recipient(checkscontract(org));

    action {
      permission_level{get_self(), name("active")},
      name(SERIESBADGE_CONTRACT),
      name("createnext"),
      series_createnext_args {
        .org = get_self(),
        .family = family,
        .badge = badge,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = onchain_lookup_data,
        .memo = memo
      }
    }.send();

    for (auto i = 0 ; i < consumers.size(); i++) {
      action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      orchestrator_addfeature_args {
        .org = get_self(),
        .badge_contract = name(SERIESBADGE_CONTRACT),
        .badge_name = badge,
        .notify_account = consumers[i],
        .memo = memo}
      }.send();
    }
  }

  ACTION org::givelatestsb (name org, name issuer, name family, name to, string memo) {
    require_auth(issuer);
    require_recipient(checkscontract(org));
    action {
    permission_level{get_self(), name("active")},
    name(SERIESBADGE_CONTRACT),
    name("issuelatest"),
    issue_latestseries_args {
      .org = get_self(),
      .family = family,
      .to = to,
      .memo = memo }
    }.send();
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
        .memo = memo }
      }.send();
    }

  }

  ACTION org::ngivesimpl(name org, name to, name badge, string memo ) {
    require_auth(get_self());
    require_recipient(name(SIMPLEBADGE_CONTRACT));
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
      .memo = memo }
    }.send();
  }


