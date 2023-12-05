#include <abnotify.hpp>

void abnotify::acreate(name org,
                name antibadge,
                name badge,
                name type,
                string offchain_lookup_data,
                string onchain_lookup_data,
                string memo) {

  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("create"),
    create_args {
      .org = org,
      .antibadge = antibadge,
      .badge = badge,
      .type = type,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo}
  }.send(); 

}

void abnotify::acinv(name org,
              name antibadge,
              name badge,
              string offchain_lookup_data,
              string onchain_lookup_data,
              string memo) {
                
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("createinv"),
    createinv_args {
      .org = org,
      .antibadge = antibadge,
      .badge = badge,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo}
  }.send(); 

}

void abnotify::aissue(name org,
              name to,
              name antibadge,
              uint64_t amount,
              string memo) {
                
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("issue"),
    issue_args {
      .org = org,
      .to = to,
      .antibadge = antibadge,
      .amount = amount,
      .memo = memo}
  }.send(); 

}

ACTION abnotify::create(name org,
              name antibadge,
              name badge,
              name type,
              string offchain_lookup_data,
              string onchain_lookup_data,
              string memo) {
  require_auth(get_self());
  require_recipient(name(ANTIBADGE_CONTRACT_NAME));              
}

ACTION abnotify::createinv(name org,
                  name antibadge,
                  name badge,
                  string offchain_lookup_data,
                  string onchain_lookup_data,
                  string memo) {
  require_auth(get_self());
  require_recipient(name(ANTIBADGE_CONTRACT_NAME));  
}

ACTION abnotify::issue(name org,
              name to,
              name antibadge,
              uint64_t amount,
              string memo) {
  require_auth(get_self());
  require_recipient(name(ANTIBADGE_CONTRACT_NAME));
}
