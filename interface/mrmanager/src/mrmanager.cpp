#include <mrmanager.hpp>

  ACTION mrmanager::initgotcha (name org,
    name authorized, 
    name badge, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string offchain_lookup_data, 
    string onchain_lookup_data,
    vector<name> consumers,
    string memo) {

    require_auth(authorized);
    
    notify_checks_contract(org);
    
    require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    
    action {
      permission_level{get_self(), name("active")},
      name(GOTCHABADGE_CONTRACT),
      name("create"),
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

  ACTION mrmanager::givegotcha (name org, name badge, name from, name to, uint64_t amount, string memo ) {
    require_auth(from);
    notify_checks_contract(org);

    action {
      permission_level{get_self(), name("active")},
      name(GOTCHABADGE_CONTRACT),
      name("give"),
      givegotcha_args {
        .org = org,
        .badge = badge,
        .from = from,
        .to = to,
        .amount = amount,
        .memo = memo }
    }.send();

  }

  ACTION mrmanager::changestart(name org, name authorized, name badge, time_point_sec new_starttime) {  
   
    require_auth(authorized);    
    notify_checks_contract(org);    
    require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    
    action {
      permission_level{get_self(), name("active")},
      name(GOTCHABADGE_CONTRACT),
      name("starttime"),
      starttime_args {
        .org = org,
        .badge = badge,
        .new_starttime = new_starttime }
    }.send();
  }

  ACTION mrmanager::changelength(name org, name authorized, name badge, uint64_t new_cycle_length) {
    require_auth(authorized);    
    notify_checks_contract(org);    
    require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    
    action {
      permission_level{get_self(), name("active")},
      name(GOTCHABADGE_CONTRACT),
      name("cyclelength"),
      cyclelength_args {
        .org = org,
        .badge = badge,
        .new_cycle_length = new_cycle_length }
    }.send();
  }

  ACTION mrmanager::changesupply(name org, name authorized, name badge, uint8_t new_supply_per_cycle) {
    require_auth(authorized);    
    notify_checks_contract(org);    
    require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    
    action {
      permission_level{get_self(), name("active")},
      name(GOTCHABADGE_CONTRACT),
      name("cyclesupply"),
      cyclesupply_args {
        .org = org,
        .badge = badge,
        .new_supply_per_cycle = new_supply_per_cycle }
    }.send();
  }


