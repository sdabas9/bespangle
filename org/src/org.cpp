#include <org.hpp>

// todo
// 1) remove check for init missing in settings for checks contract.

  ACTION org::initsystem(name org, name checks_contract, vector<name> producers, name aacollection) {
    require_auth (org);
    require_recipient(ORCHESTRATOR_CONTRACT);
    require_recipient(AABADGE_CONTRACT);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("chkscontract"),
      chkscontract_args {
        .checks_contract = checks_contract}
    }.send();
  }

  ACTION org::chkscontract (name checks_contract) {
    require_auth(get_self());
    settings_table _settings( get_self(), get_self().value );
    auto itr = _settings.find(1);
    if(itr == _settings.end()) {
      _settings.emplace(get_self(), [&](auto& row) {
        row.id = 1;
        row.checks_contract = checks_contract;
      });
    } else {
      _settings.modify(itr, get_self(),[&](auto& row) {
        row.checks_contract = checks_contract;
      });
    }
  }

  ACTION org::initsimple (name creator, 
    name org,
    name badge, 
    string ipfs_image, 
    string display_name, 
    vector<name> consumers) {
    require_auth(creator);

    require_recipient(checkscontract());

    action {
      permission_level{get_self(), name("active")},
      name(SIMPLEBADGE_CONTRACT),
      name("create"),
      createsimple_args {
        .org = get_self(),
        .badge = badge,
        .ipfs_image = ipfs_image,
        .memo = display_name}
    }.send();

    for (auto i = 0 ; i < consumers.size(); i++) {
      action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      orchestrator_addfeature_args {
        .org = get_self(),
        .badge_contract = name(SIMPLEBADGE_CONTRACT),
        .badge_name = badge,
        .notify_account = consumers[i],
        .memo = display_name}
      }.send();
    } 
  }

  ACTION org::initgotcha (name creator, 
    name badge, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string ipfs_image, 
    string display_name, 
    vector<name> consumers) {

    require_auth(creator);
    
    require_recipient(checkscontract());
    
    action {
      permission_level{get_self(), name("active")},
      name(GOTCHABADGE_CONTRACT),
      name("create"),
      creategotcha_args {
        .org = get_self(),
        .badge = badge,
        .starttime = starttime,
        .cycle_length = cycle_length,
        .supply_per_cycle = supply_per_cycle,
        .ipfsimage = ipfs_image,
        .memo = display_name }
    }.send();

    for (auto i = 0 ; i < consumers.size(); i++) {
      action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      orchestrator_addfeature_args {
        .org = get_self(),
        .badge_contract = name(GOTCHABADGE_CONTRACT),
        .badge_name = badge,
        .notify_account = consumers[i],
        .memo = display_name}
      }.send();
    }
  }

  ACTION org::givegotcha (name badge, name from, name to, uint8_t amount, string memo ) {
    require_auth(from);
    require_recipient(checkscontract());

    action {
      permission_level{get_self(), name("active")},
      name("gotchabadge"),
      name("give"),
      givegotcha_args {
        .org = get_self(),
        .badge = badge,
        .from = from,
        .to = to,
        .amount = amount,
        .memo = memo }
    }.send();

  }

  ACTION org::givesimple (name badge, name authorizer, name to, string memo ) {
    require_auth(authorizer);

    require_recipient(checkscontract());

    action {
      permission_level{get_self(), name("active")},
      name(SIMPLEBADGE_CONTRACT),
      name("issue"),
      issuesimple_args {
        .org = get_self(),
        .to = to,
        .badge = badge,
        .memo = memo }
    }.send();
  }

  ACTION org::initround (name authorizer, name round, string display_name) {
    require_auth(authorizer);
    require_recipient(checkscontract());

    action {
      permission_level{get_self(), name("active")},
      name(ROUNDS_CONTRACT),
      name("createround"),
      createround_args {
        .org = get_self(),
        .round = round,
        .description = display_name,
        .account_constrained = false}
    }.send();
  }

  ACTION org::startround (name authorizer, name round) {
    require_auth(authorizer);
    require_recipient(checkscontract());

    action {
      permission_level{get_self(), name("active")},
      name(ROUNDS_CONTRACT),
      name("startround"),
      startround_args {
        .org = get_self(),
        .round = round}
    }.send();
  }
  
  ACTION org::endround (name authorizer, name round) {
    require_auth(authorizer);
    require_recipient(checkscontract());

    action {
      permission_level{get_self(), name("active")},
      name(ROUNDS_CONTRACT),
      name("endround"),
      endround_args {
        .org = get_self(),
        .round = round}
    }.send();
  }

  ACTION org::addbdgetornd (name authorizer,
    name round, 
    uint64_t badge_id, 
    name balance_based_scoring_type, 
    uint16_t balance_based_scoring_weight,
    name source_based_scoring_type,
    uint16_t source_based_scoring_weight) {
    
    require_auth(authorizer);
    require_recipient(checkscontract());  

    action {
      permission_level{get_self(), name("active")},
      name(ROUNDS_CONTRACT),
      name("addscoremeta"),
      addscoremeta_args {
        .org = get_self(),
        .round = round,
        .badge_id = badge_id,
        .balance_based_scoring_type = balance_based_scoring_type,
        .balance_based_scoring_weight =balance_based_scoring_weight,
        .source_based_scoring_type = source_based_scoring_type,
        .source_based_scoring_weight = source_based_scoring_weight
        }
    }.send();
  }

