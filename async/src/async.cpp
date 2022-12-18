#include <async.hpp>

ACTION async::qgivesimpl(name org, name to, name badge, string memo) {
  require_auth(name(ORG_INTERFACE_CONTRACT));
  queue_give_simple(org, to, badge, memo); 
}

ACTION async::process(name org, uint8_t records_to_process) {
  process_simple( org,  records_to_process);
}

/*ACTION async::process(name org, uint8_t records_to_process) {
  require_auth(get_self());
  //uint8_t records_processed = 0;
  process_simple( org,  records_to_process);
  //int process_max = records_to_process;
  //process(org, records_to_process);
 // while( process_max > 0) {
 //   process_max = process_max - process(org, process_max);
 // }
} */


/*int process(name org, int process_max) {
  int processed = 0;
  string currspot = getcurrspot();
  switch(currspot)
    case "givesimple" => processed = process_simple(org, process_max);
    case "allprocessed" => processed = process_max;
  return processed;
}*/

/*string getcurrspot(name org) {
  givesimple_table _givesimple(get_self(), org.value);
  if(_givesimple.begin() != _givesimple.end()) {
    return "givesimple";
  } else {
    return "allprocessed";
  }
}*/



