#include <notification.hpp>


void notification::aaddclaimer (name org, 
  name account, 
  name assetname, 
  uint64_t account_cap,
  string memo) {

  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("addclaimer"),
    addclaimer_args {
      .org = org,
      .account = account,
      .assetname = assetname,
      .account_cap = account_cap,
      .memo = memo }
  }.send(); 

}

void notification::baddclaimer (name org, 
  name account, 
  name assetname, 
  uint64_t account_cap,
  string memo) {

  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("addclaimer"),
    addclaimer_args {
      .org = org,
      .account = account,
      .assetname = assetname,
      .account_cap = account_cap,
      .memo = memo }
  }.send(); 

}

ACTION notification::addclaimer(name org, name account, name assetname, uint64_t account_cap, string memo) {
  require_auth(get_self());
  require_recipient(name(CLAIMASSET_CONTRACT_NAME));
}

void notification::ainitclaim (name org, 
  name assetname, 
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  string memo) {

  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("initcasset"),
    create_claimasset_args {
      .org = org,
      .assetname = assetname,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
  }.send();  

}

ACTION notification::initcasset(name org, 
  name assetname,
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  string memo) {
  require_auth(get_self());
  require_recipient(name(CLAIMASSET_CONTRACT_NAME));
}

void notification::aclaimasset(name org, 
      name to, 
      name assetname, 
      string memo ) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("claimasset"),
    claimasset_args {
      .org = org,
      .to = to,
      .assetname = assetname,
      .memo = memo }
  }.send();  
}

ACTION notification::claimasset(name org, 
  name to, 
  name assetname, 
  string memo ) {
  require_auth(get_self());
  require_recipient(name(CLAIMASSET_CONTRACT_NAME));
}

void notification::aaddfeature (name org, 
      name badge_name,
      name notify_account,
      string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = badge_name,
      .notify_account = notify_account,
      .memo = memo }
  }.send();     
}

void notification::baddfeature (name org, 
      name badge_name,
      name notify_account,
      string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = badge_name,
      .notify_account = notify_account,
      .memo = memo }
  }.send();     
}

ACTION notification::addfeature(name org, 
      name badge_name,
      name notify_account,
      string memo) {
  require_auth(get_self());
  require_recipient(name(ORCHESTRATOR_CONTRACT_NAME)); 
}



void notification::aaddroun(name org, name round, string description) {
  action(
    permission_level{get_self(), "active"_n},
    get_self(), "addround"_n,
    addround_args{.org = org, .round = round, .description = description}
  ).send();
}
ACTION notification::addround(name org, name round, string description) {
  require_auth(get_self());
  require_recipient(name(BOUNDED_AGG_CONTRACT_NAME));
}

void notification::asetstatu(name org, name round, name status, time_point execution_time) {
  action(
    permission_level{get_self(), "active"_n},
    get_self(), "setstatus"_n,
    setstatus_args{.org = org, .round = round, .status = status, .execution_time = execution_time}
  ).send();
}
ACTION notification::setstatus(name org, name round, name status, time_point execution_time) {
  require_auth(get_self());
  require_recipient(name(DEFERRED_CONTRACT_NAME));
}

void notification::aaddbadg(name org, name round, name badge, time_point execution_time) {
    action(
      permission_level{get_self(), "active"_n},
      get_self(), "addbadge"_n,
      addbadge_args{.org = org, .round = round, .badge = badge, .execution_time = execution_time}
    ).send();
}
ACTION notification::addbadge(name org, name round, name badge, time_point execution_time) {
  require_auth(get_self());
  require_recipient(name(DEFERRED_CONTRACT_NAME));
}

void notification::arembadg(name org, name round, name badge, time_point execution_time) {
    action(
      permission_level{get_self(), "active"_n},
      get_self(), "rembadge"_n,
      rembadge_args{.org = org, .round = round, .badge = badge, .execution_time = execution_time}
    ).send();
}
ACTION notification::rembadge(name org, name round, name badge, time_point execution_time) {
  require_auth(get_self());
  require_recipient(name(DEFERRED_CONTRACT_NAME));
}

void notification::acreateredee(name org,
  name antibadge,
  name badge,
  name type,
  string offchain_lookup_data,
  string onchain_lookup_data,
  string memo) {
    action(
      permission_level{get_self(), "active"_n},
      get_self(), "createredeem"_n,
      createredeem_args{
        .org = org, 
        .antibadge = antibadge,
        .badge = badge,
        .type = type,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = onchain_lookup_data,
        .memo =  memo
      }
    ).send();        
}
ACTION notification::createredeem(name org,
  name antibadge,
  name badge,
  name type,
  string offchain_lookup_data,
  string onchain_lookup_data,
  string memo) {
  require_auth(get_self());
  require_recipient(name(ANTIBADGE_CONTRACT_NAME));
}

void notification::ainitfixe(name org, name round, name badge, uint64_t rate, name notification_contract) {
  action(
    permission_level{get_self(), "active"_n},
    get_self(), "initfixed"_n,
    initfixed_args {
      .org = org,
      .round = round,
      .badge = badge,
      .rate = rate,
      .notification_contract = notification_contract
    }
  ).send();
}
ACTION notification::initfixed(name org, name round, name badge, uint64_t rate, name notification_contract) {
  require_auth(get_self());
  require_recipient(name(REDEEM_CONTRACT_NAME));
}

void notification::ainitdynami(name org, name round, name badge, uint64_t amount, name notification_contract) {
  action(
    permission_level{get_self(), "active"_n},
    get_self(), "initdynamic"_n,
    initdynamic_args {
      .org = org,
      .round = round,
      .badge = badge,
      .amount = amount,
      .notification_contract = notification_contract
    }
  ).send();
}
ACTION notification::initdynamic(name org, name round, name badge, uint64_t amount, name notification_contract) {
  require_auth(get_self());
  require_recipient(name(REDEEM_CONTRACT_NAME));
}
