#include <async.hpp>

ACTION async::qgivesimpl(name org, name to, name badge, string memo) {
  require_auth(name(ORG_INTERFACE_CONTRACT));
  queue_give_simple(org, to, badge, memo); 
}

ACTION async::process(name org, uint8_t records_to_process) {
  process_give_simple(org, records_to_process);
}

ACTION async::isolate(name org) {
  require_auth(get_self());
  isolate_give_simple(org);
}