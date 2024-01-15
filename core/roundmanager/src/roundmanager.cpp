#include <roundmanager.hpp>

// add badges
// add redeemables
// 
ACTION defineround(name org, 
  name authorized_account, 
  name round,
  string round_description, 
  vector<name> badges, 
  vector<redeemable> redeem_badge_info,
  time_point_sec round_start_time,
  time_point_sec round_end_time,
  uint64_t claim_duration_secs
  ) {
  
  require_auth(authorized_account);

  notify_checks_contract(org);
  require_recipient(name(ROUND_MANAGER_VALIDATION_CONTRACT));

  check(round_start_time >= current_time_point(), "round_start_time must be greater than current time");
  check(round_end_time >= current_time_point(), "round_end_time must be greater than current time");
  check(round_start_time < round_end_time, "round_end_time must be greater than round_start_time");
 // check(claim_start_time >= current_time_point(), "claim_start_time must be greater than current time");
  
  time_point_sec claim_start_time = round_end_time;
  time_point_sec claim_end_time = claim_start_time + claim_duration_secs;

  // init round
  // future start time
  // future end time
  ACTION rqsetstat(name org, name round, name status, time_point execution_time);
  ACTION rqaddbgrnd(name org, name round, name badge, time_point execution_time);
  ACTION rqrembgrnd(name org, name round, name badge, time_point execution_time);
  action(
    permission_level{get_self(), "active"_n},
    name(BOUNDED_AGG_CONTRACT_NAME), "addrnd"_n,
    addround_args{.org = org, .round = round, .description = round_description}
  ).send();

  action(
    permission_level{get_self(), "active"_n},
    name(ROUND_DEFERRED_CONTRACT), "rqsetstat"_n,
    setstatus_args{.org = org, .round = round, .status = name("ongoing"), .execution_time = round_start_time}
  ).send();

  action(
    permission_level{get_self(), "active"_n},
    name(ROUND_DEFERRED_CONTRACT), "rqsetstat"_n,
    setstatus_args{.org = org, .round = round, .status = name("end"), .execution_time = claim_end_time}
  ).send();

  for(auto i = 0 ; i < badges.size(); i++ ) {
    name badge = badges[i];

    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      addfeature_args {
        .org = org,
        .badge = badge,
        .notify_account = name(BOUNDED_AGG_CONTRACT_NAME),
        .memo = ""}
    }.send();
   
    action(
      permission_level{get_self(), "active"_n},
      name(ROUND_DEFERRED_CONTRACT), "rqaddbgrnd"_n,
      addbadge_args{.org = org, .round = round, .badge = badge, .execution_time = round_start_time}
    ).send();

    action(
      permission_level{get_self(), "active"_n},
      name(ROUND_DEFERRED_CONTRACT), "rqrembgrnd"_n,
      rembadge_args{.org = org, .round = round, .badge = badge, .execution_time = round_end_time}
    ).send();
  }

  
  
  for(auto i = 0; i < redeem_badge_info.size(); i++) {

    name redeem_badge  = redeem_badge_info[i].redeem_badge;
    name reward_yielding_badge = redeem_badge_info[i].reward_yielding_badge;
    time_point redeem_start_time = redeem_badge_info[i].redeem_start_time;
    time_point redeem_end_time = redeem_badge_info[i].redeem_end_time;

    check(redeem_start_time < redeem_end_time, "redeem_start_time should be less than redeem_end_time");
    check(redeem_start_time >= round_start_time, "redeem_start_time should be greater or same as round_start_time");
    check(redeem_end_time <= round_end_time, "accumulation_end_time should be less than or same as round_end_time");

    action(
      permission_level{get_self(), "active"_n},
      get_self(), "createredeem"_n,
      createredeem_args{
        .org = org, 
        .antibadge = redeem_badge,
        .badge = reward_yielding_badge,
        .type = name("redeem"),
        .offchain_lookup_data = redeem_badge_info[i].offchain_lookup_data,
        .onchain_lookup_data = redeem_badge_info[i].onchain_lookup_data,
        .memo =  ""
      }
    ).send();

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addfeature"),
      addfeature_args {
        .org = org,
        .badge_name = reward_yielding_badge,
        .notify_account = name(BOUNDED_AGG_CONTRACT_NAME),
        .memo = ""}
    }.send();

    action(
      permission_level{get_self(), "active"_n},
      get_self(), "addbadge"_n,
      addbadge_args{.org = org, .round = round, .badge = redeem_badge, .execution_time = redeem_badge_info[i].redeem_start_time}
    ).send();

    action(
      permission_level{get_self(), "active"_n},
      get_self(), "rembadge"_n,
      rembadge_args{.org = org, .round = round, .badge = redeem_badge, .execution_time = redeem_badge_info[i].redeem_end_time}
    ).send();

    if (redeem_badge_info[i].type == name("fixed")) {
      check(redeem_badge_info[i].rate>0, "For fixed type rate should be > 0");
      action(
        permission_level{get_self(), "active"_n},
        get_self(), "initfixed"_n,
        initfixed_args {
          .org = org,
          .round = round,
          .badge = reward_yielding_badge,
          .rate = redeem_badge_info[i].rate,
          .notification_contract = redeem_badge_info[i].reward_pool_contract
        }
      ).send();

    } else if (redeem_badge_info[i].type == name("dynamic")) {
      action(
        permission_level{get_self(), "active"_n},
        get_self(), "initdynamic"_n,
        initdynamic_args {
          .org = org,
          .round = round,
          .badge = reward_yielding_badge,
          .amount = 1000000,
          .notification_contract = redeem_badge_info[i].reward_pool_contract
        }
      ).send();
      
      for(auto j = 0; j < badges_info.size(); j++) {
        if(badges_info[j].badge == reward_yielding_badge) {
          check(badges_info[j].accumulation_end_time <  redeem_badge_info[i].redeem_start_time, "for dynamic type, accumulation_end_time should be less than redeem_start_time");
          break;
        }
      }
      
    } else {
      check(false, "only supports fixed or dynamic type");
    }
  }

}



ACTION roundmanager::addround(name org, name round, string description){
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION roundmanager::setstatus(name org, name round, name status, time_point execution_time) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION roundmanager::addbadge(name org, name round, name badge, time_point execution_time) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION roundmanager::rembadge(name org, name round, name badge, time_point execution_time) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION roundmanager::createredeem(name org,
               name antibadge,
               name badge,
               name type,
               string offchain_lookup_data,
               string onchain_lookup_data,
               string memo) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION roundmanager::initfixed(name org, name round, name badge, uint64_t rate, name notification_contract) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION roundmanager::initdynamic(name org, name round, name badge, uint64_t amount, name notification_contract) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION roundmanager::addfeature(name org, name badge_name, name notify_account, string memo) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}


