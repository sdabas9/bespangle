#include <notification.hpp>

void notification::ainitsimple (name org, 
  name badge, 
  vector<name> parent_badges, 
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  string memo) {

  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("initsimple"),
    create_simplebadge_args {
      .org = org,
      .badge = badge,
      .parent_badges = parent_badges,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
  }.send();   

}

void notification::asimpleissue (name org, 
  name to, 
  name badge,
  string memo) {

  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("issuesimple"),
    issue_args {
      .org = org,
      .to = to,
      .badge = badge,
      .memo = memo}
  }.send(); 

}
  
void notification::bsimpleissue (name org, 
  name to, 
  name badge, 
  string memo) {
  
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("issuesimple"),
    issue_args {
      .org = org,
      .to = to,
      .badge = badge,
      .memo = memo}
  }.send(); 

}



void notification::aaddclaimer (name org, 
  name account, 
  name assetname, 
  uint64_t account_cap) {

  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("addclaimer"),
    addclaimer_args {
      .org = org,
      .account = account,
      .assetname = assetname,
      .account_cap = account_cap }
  }.send(); 

}

void notification::ainitclaim (name org, 
  name assetname, 
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  string memo) {

  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("initclaim"),
    create_claimasset_args {
      .org = org,
      .assetname = assetname,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
  }.send();  

}

ACTION notification::initsimple(name org, 
  name badge, 
  vector<name> parent_badges, 
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  string memo) {
  require_auth(get_self());
  require_recipient(name(SIMPLEBADGE_CONTRACT_NAME));
}

ACTION notification::issuesimple(name org, name to, name badge, string memo ) {
  require_auth(get_self());
  require_recipient(name(SIMPLEBADGE_CONTRACT_NAME));
}

ACTION notification::addclaimer(name org, name account, name assetname, uint64_t account_cap) {
  require_auth(get_self());
  require_recipient(name(CLAIMASSET_CONTRACT_NAME));
}

ACTION notification::initclaim(name org, 
  name assetname,
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  string memo) {
  require_auth(get_self());
  require_recipient(name(CLAIMASSET_CONTRACT_NAME));
}


