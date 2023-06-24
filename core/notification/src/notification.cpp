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

ACTION notification::initsimple(name org, 
  name badge, 
  vector<name> parent_badges, 
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  string memo) {
  require_auth(get_self());
  require_recipient(name(SIMPLEBADGE_CONTRACT_NAME));
}

void notification::asimpleissue (name org, 
  name to, 
  name badge,
  uint64_t amount,
  string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("givesimple"),
    issue_args {
      .org = org,
      .to = to,
      .badge = badge,
      .amount = amount,
      .memo = memo}
  }.send(); 

}
  
void notification::bsimpleissue (name org, 
  name to, 
  name badge,
  uint64_t amount, 
  string memo) {
  
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("givesimple"),
    issue_args {
      .org = org,
      .to = to,
      .badge = badge,
      .amount = amount,
      .memo = memo}
  }.send(); 

}

ACTION notification::givesimple(name org, name to, name badge, uint64_t amount, string memo ) {
  require_auth(get_self());
  require_recipient(name(SIMPLEBADGE_CONTRACT_NAME));
}

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

ACTION notification::addfeature(name org, 
      name badge_name,
      name notify_account,
      string memo) {
  require_auth(get_self());
  require_recipient(name(ORCHESTRATOR_CONTRACT_NAME)); 
}

void notification::acreateserie(name org, name series) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("createseries"),
    createseries_args {
      .org = org,
      .series = series }
  }.send();  
}

ACTION notification::createseries(name org, name series) {
  require_auth(get_self());
  require_recipient(name(SERIESBADGE_CONTRACT_NAME));
}

void notification::acreatenex(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("createnext"),
    createnext_args {
      .org = org,
      .series = series,
      .badge = badge,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
  }.send();  
}

ACTION notification::createnext(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
  require_auth(get_self());
  require_recipient(name(SERIESBADGE_CONTRACT_NAME));    
}


void notification::aissuelates(name org, name series, name to, uint64_t count, string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("issuelatest"),
    issuelatest_args {
      .org = org,
      .series = series,
      .to = to,
      .count = count,
      .memo = memo }
  }.send();    
}

ACTION notification::issuelatest(name org, name series, name to, uint64_t count, string memo) {
  require_auth(get_self());
  require_recipient(name(SERIESBADGE_CONTRACT_NAME));    
}

void notification::aissuean(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("issueany"),
    issueany_args {
      .org = org,
      .series = series,
      .seq_id = seq_id,
      .to = to,
      .count = count,
      .memo = memo }
  }.send();      
}

ACTION notification::issueany(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo) {
  require_auth(get_self());
  require_recipient(name(SERIESBADGE_CONTRACT_NAME));      
}




