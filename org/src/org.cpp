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

  ACTION org::linksyschk(name org, name system_check_contract) {
    require_auth(org);
    availablechk_table availablechk(get_self(), get_self().value);
    auto available_itr = availablechk.require_find(system_check_contract.value, "Unsupported system_check_contract");

    
    systemchecks_table systemchecks(get_self(), get_self().value);
    auto itr = systemchecks.find(org.value);
    if (itr != systemchecks.end()) {
        systemchecks.modify(itr, get_self(), [&](auto& row) {
            row.system_check_contracts.push_back(system_check_contract);
        });
    } else {
        systemchecks.emplace(get_self(), [&](auto& row) {
            row.org = org;
            row.system_check_contracts.push_back(system_check_contract);
        });
    }
  }

  ACTION org::unlinksyschk(name org, name system_check_contract) {
    require_auth(org);
    systemchecks_table systemchecks(get_self(), get_self().value);
    auto itr = systemchecks.find(org.value);
    if (itr != systemchecks.end()) {
        systemchecks.modify(itr, get_self(), [&](auto& row) {
            auto it = std::find(row.system_check_contracts.begin(), row.system_check_contracts.end(), system_check_contract);
            if (it != row.system_check_contracts.end()) {
                row.system_check_contracts.erase(it);
            }
        });
    }
  }

  ACTION org::addsyscheck(name system_check_contract) {
    require_auth (get_self());
    availablechk_table availablechk(get_self(), get_self().value);
    availablechk.emplace(get_self(), [&](auto& row) {
        row.system_check_contract = system_check_contract;
    });
  }

  ACTION org::remsyscheck(name system_check_contract) {
    require_auth (get_self());
    availablechk_table availablechk(get_self(), get_self().value);
    auto itr = availablechk.find(system_check_contract.value);
    if (itr != availablechk.end()) {
        availablechk.erase(itr);
    }
  }

  ACTION org::processsync (name org, name action, name authorizer) {
    require_auth(authorizer); 
    require_recipient(checkscontract(org));
    linked_inbuilt_checks_contract(org);
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
    linked_inbuilt_checks_contract(org);
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

  ACTION org::naddfeatur (name org,
    name badge_name, 
    name notify_account, 
    string memo) {
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
    linked_inbuilt_checks_contract(org);


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
    linked_inbuilt_checks_contract(org);

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
    linked_inbuilt_checks_contract(org);
    
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


