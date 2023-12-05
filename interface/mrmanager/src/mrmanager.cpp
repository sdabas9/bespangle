#include <mrmanager.hpp>

  ACTION mrmanager::naddfeatur (name org,
    name badge_name, 
    name notify_account, 
    string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT));
  }

  ACTION mrmanager::initgotcha (name org,
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
    
    notify_checks_contract(org);
    require_recipient(name(MUTUAL_RECOGNITION_VALIDATION_CONTRACT));
    
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
      addfeature_args {
        .org = get_self(),
        .badge_name = badge,
        .notify_account = consumers[i],
        .memo = memo}
      }.send();
    }
  }

  ACTION mrmanager::ninitgotch (name org, 
    name badge, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string offchain_lookup_data, 
    string onchain_lookup_data,
    vector<name> consumers,
    string memo) {

    require_auth(get_self());
    require_recipient(name(MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT));

  }


  ACTION mrmanager::givegotcha (name org, name badge, name from, name to, uint8_t amount, string memo ) {
    require_auth(from);
    notify_checks_contract(org);

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

  ACTION mrmanager::ngivegotch (name org, name badge, name from, name to, uint8_t amount, string memo) {
    require_auth(get_self());
    require_recipient(name(MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT));
  }

