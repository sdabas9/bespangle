#include <roundmanager.hpp>



ACTION roundmanager::createround(name org, name round, string description) {
  // action to create round
}

// creates a round
// add badges to round
// add redeemable badges to round

ACTION roundmanager::defineround(name org, name authorized_account, 
  name round, 
  string round_description, 
  vector<badges> badges_info, 
  vector<redeemable> redeem_badge_info,
  time_point round_start_time, 
  time_point round_end_time,
  ) {
  
  require_auth(authorized_account);
  notify_checks_contract(org);
  notify_linked_inbuilt_checks_contract(org);

  // set status of round to ongoing in future 
  // action to add badges to round in future.
  // action to remove badges from round in future.
  for(auto i = 0 ; i < badges.size(); i++ ) {
    name badge = badges[i].badge;
    time_point accumulation_start_time = badges[i].accumulation_start_time;
    time_point accumulation_end_time = badges[i].accumulation_end_time;
    action(
      permission_level{get_self(), "active"_n},
      get_self(), "addbadge"_n,
      addbadge_args{.org = org, .round = round, .badge = badge, .execution_time = accumulation_start_time}
    ).send();

    action(
      permission_level{get_self(), "active"_n},
      get_self(), "rembadge"_n,
      rembadge_args{.org = org, .round = round, .badge = badge, .execution_time = accumulation_end_time}
    ).send();
  }

  
  
  for(auto i = 0; i < redeemable.size(); i++) {

    name redeem_badge  = redeemable[i].redeem_badge;
    name reward_yielding_badge = redeemable[i].reward_yielding_badge;

    action(
      permission_level{get_self(), "active"_n},
      get_self(), "redeemcr"_n,
      redeemcr_args{
        .org = org, 
        .antibadge = redeem_badge,
        .badge = reward_yielding_badge,
        .type = name("redeem"),
        .offchain_lookup_data = redeemable[i].offchain_lookup_data,
        .onchain_lookup_data = redeemable[i].onchain_lookup_data,
        .memo =  ""
      }
    ).send();

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addfeature"),
      addfeature_args {
        .org = org,
        .badge_name = badge,
        .notify_account = name(BOUNDED_AGG_CONTRACT_NAME),
        .memo = memo}
    }.send();

    action(
      permission_level{get_self(), "active"_n},
      get_self(), "addbadge"_n,
      addbadge_args{.org = org, .round = round, .badge = redeem_badge, .execution_time = redeemable[i].redeem_start_time}
    ).send();

    action(
      permission_level{get_self(), "active"_n},
      get_self(), "rembadge"_n,
      rembadge_args{.org = org, .round = round, .badge = redeem_badge, .execution_time = redeemable[i].redeem_end_time}
    ).send();

    if (type == name("fixed")) {
      check(rate>0, "For fixed type rate should be > 0");
      action(
        permission_level{get_self(), "active"_n},
        get_self(), "initfixed"_n,
        initfixed_args {
          .org = org,
          .round = round,
          .badge = reward_yielding_badge,
          .rate = redeemable[i].rate,
          .notification_contract = redeemable[i].reward_pool_contract
        }
      ).send();

    } else if (type == name("dynamic")) {
      action(
        permission_level{get_self(), "active"_n},
        get_self(), "initdynamic"_n,
        initdynamic_args {
          .org = org,
          .round = round,
          .badge = reward_yielding_badge,
          .amount = 1000000,
          .notification_contract = redeemable[i].reward_pool_contract
        }
      ).send();
    } else {
      check(false, "only supports fixed or dynamic type");
    }
  }

}

ACTION roundmanager::reward(name org, name round, name badge, name reward_badge, time_point start_time, time_point end_time) {
  // action to add badge to round in future
  // action to remove badge from round in future
  // update status of round to close.
}

ACTION roundmanager::claimreward(name org, ){
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

ACTION roundmanager::redeemcr(name org,
               name antibadge,
               name badge,
               name type,
               string offchain_lookup_data,
               string onchain_lookup_data,
               string memo) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION initfixed(name org, name round, name badge, uint64_t rate, name notification_contract) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}

ACTION initdynamic(name org, name round, name badge, uint64_t amount, name notification_contract) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT_NAME));
}


