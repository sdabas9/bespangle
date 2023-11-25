#include <mrnotify.hpp>

void mrnotify::ainitmr (name org, 
  name badge, 
  time_point_sec starttime, 
  uint64_t cycle_length, 
  uint8_t supply_per_cycle, 
  string offchain_lookup_data,
  string onchain_lookup_data,  
  string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("initmr"),
    create_args {
      .org = org,
      .badge = badge,
      .starttime = starttime,
      .cycle_length = cycle_length,
      .supply_per_cycle = supply_per_cycle,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
  }.send();  
}

ACTION mrnotify::initmr(name org, 
  name badge, 
  time_point_sec starttime, 
  uint64_t cycle_length, 
  uint8_t supply_per_cycle, 
  string offchain_lookup_data,
  string onchain_lookup_data,  
  string memo) {
  require_auth(get_self());
  require_recipient(name(MUTUAL_RECOGNITION_CONTRACT_NAME));
}

void mrnotify::aissuemr (name org, 
  name badge, 
  name from, 
  name to, 
  uint8_t amount, 
  string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("givemr"),
    issue_args {
      .org = org,
      .badge = badge,
      .from = from,
      .to = to,
      .amount = amount,
      .memo = memo}
  }.send(); 

}
  

ACTION mrnotify::givemr(name org, 
      name badge, 
      name from, 
      name to, 
      uint8_t amount, 
      string memo) {
  require_auth(get_self());
  require_recipient(name(MUTUAL_RECOGNITION_CONTRACT_NAME));
}