#include <simnotify.hpp>

void simnotify::ainitsimple (name org, 
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

ACTION simnotify::initsimple(name org, 
  name badge, 
  vector<name> parent_badges, 
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  string memo) {
  require_auth(get_self());
  require_recipient(name(SIMPLEBADGE_CONTRACT_NAME));
}

void simnotify::asimpleissue (name org, 
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
  
void simnotify::bsimpleissue (name org, 
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

ACTION simnotify::givesimple(name org, name to, name badge, uint64_t amount, string memo ) {
  require_auth(get_self());
  require_recipient(name(SIMPLEBADGE_CONTRACT_NAME));
}
