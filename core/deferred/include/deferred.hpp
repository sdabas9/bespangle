#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

#define NOTIFICATION_CONTRACT_NAME "notification"

#define ROUND_SET_STATUS_NOTIFICATION NOTIFICATION_CONTRACT_NAME"::setstatus"
#define ROUND_ADD_BADGE_NOTIFICATION NOTIFICATION_CONTRACT_NAME"::addbadge"
#define ROUND_REMOVE_BADGE_NOTIFICATION NOTIFICATION_CONTRACT_NAME"::rembadge"

CONTRACT deferred : public contract {
public:
  using contract::contract;

  [[eosio::on_notify(ROUND_SET_STATUS_NOTIFICATION)]]
  void rxsetstat(name org, name round, name status, time_point execution_time);

  [[eosio::on_notify(ROUND_ADD_BADGE_NOTIFICATION)]]
  void rxaddbgrnd(name org, name round, name badge, time_point execution_time);

  [[eosio::on_notify(ROUND_REMOVE_BADGE_NOTIFICATION)]] 
  void rxrembgrnd(name org, name round, name badge, time_point execution_time);  

  ACTION rqsetstat(name org, name round, name status, time_point execution_time);
  ACTION rqaddbgrnd(name org, name round, name badge, time_point execution_time);
  ACTION rqrembgrnd(name org, name round, name badge, time_point execution_time);

  ACTION resetstat(name org, name round, name status);
  ACTION readdbgrnd(name org, name round, name badge);
  ACTION rerembgrnd(name org, name round, name badge);

  ACTION rounddeque(name org, uint64_t step, name executing_account);
   

  /*ACTION addrecord(name org, name contract, name from, name to, uint64_t amount, std::string memo, name badge, time_point_sec execution_time);
  ACTION execute(name org, uint64_t record_id);
  ACTION execbyhour(name org, uint64_t hour, uint64_t step);*/




  struct issue_args {
    name org;
    name badge;
    name from;
    name to;
    uint64_t amount;
    std::string memo;
  };

  struct setstat_args {
    name org;
    name round;
    name status;
  };

  struct addbgrnd_args {
    name org;
    name round;
    name badge;
  };

  struct rembgrnd_args {
    name org;
    name round;
    name badge;
  };

  struct rqsetstat_args {
    name org;
    name round;
    name status;
    time_point execution_time;
  };

  struct rqaddbgrnd_args {
    name org;
    name round;
    name badge;
    time_point execution_time;
  };

  struct rqrembgrnd_args {
    name org;
    name round;
    name badge;
    time_point execution_time;
  };

private:

  TABLE rounds {
    uint64_t id;
    uint64_t execution_time_in_micro_sec_since_epoch;
    name action;
    name param1;
    name param2;
    uint64_t primary_key() const { return id; }
    uint64_t microsec() const { return execution_time_in_micro_sec_since_epoch; }
  };

  typedef eosio::multi_index<name("rounds"), rounds,
    indexed_by<name("microindex"), const_mem_fun<rounds, uint64_t, &rounds::microsec>>
  > rounds_table;



  /*uint16_t execute_a(name org, uint16_t step) {
    eosio::time_point tp = eosio::current_time_point();
    eosio::microseconds micros = tp.time_since_epoch();
    uint64_t count_micros = micros.count();
    
    records_table records(get_self(), org.value);
    auto micro_index = records.get_index<name("microindex")>();
    auto lower = micro_index.lower_bound(1);
    auto upper = micro_index.upper_bound(count_micros);

    uint16_t processed = 0;
    for(auto itr = lower; itr != upper; ++itr) {
       
      action(
        permission_level{get_self(), "active"_n},
        get_self(), "notifya"_n,
        issue_args{org, iterator->badge, iterator->from, iterator->to, iterator->amount, iterator->memo}
      ).send();

      processed++;
      records.erase(itr);

      if(processed == step) {
        return processed;
      }
      
    }
    return processed;
  }


  uint16_t execute_b(name org, uint16_t step) {
    eosio::time_point tp = eosio::current_time_point();
    eosio::microseconds micros = tp.time_since_epoch();
    uint64_t count_micros = micros.count();
    
    records_table records(get_self(), org.value);
    auto micro_index = records.get_index<name("microindex")>();
    auto lower = micro_index.lower_bound(1);
    auto upper = micro_index.upper_bound(count_micros);

    uint16_t processed = 0;
    for(auto itr = lower; itr != upper; ++itr) {
       
      action(
        permission_level{get_self(), "active"_n},
        get_self(), "notifyb"_n,
        issue_args{org, iterator->badge, iterator->from, iterator->to, iterator->amount, iterator->memo}
      ).send();

      processed++;
      records.erase(itr);

      if(processed == step) {
        return processed;
      }
      
    }
    return processed;
  }*/

  /*TABLE records {
    uint64_t id;
    uint64_t execution_time_in_micro_sec_since_epoch;
    name contract;
    uint64_t primary_key() const { return id; }
    uint64_t microsec() const { return execution_time_in_micro_sec_since_epoch; }
  };

  typedef eosio::multi_index<name("records"), record,
    indexed_by<name("microindex"), const_mem_fun<record, uint64_t, &record::microsec>>
  > records_table;


  TABLE record {
    uint64_t id;
    uint64_t hour;
    name contract;
    name from;
    name to;
    uint64_t amount;
    std::string memo;
    name badge;
    time_point_sec created_at;

    uint64_t primary_key() const { return id; }
    uint64_t by_hour() const { return hour; }
  };

  typedef eosio::multi_index<name("records"), record,
    indexed_by<name("hourindex"), const_mem_fun<record, uint64_t, &record::by_hour>>
  > records_table;*/

  uint64_t get_hour_since_epoch(const time_point_sec& execution_time) {
    const uint64_t hour_since_epoch = execution_time.sec_since_epoch () / 3600;
    return hour_since_epoch;
  }

  uint64_t get_current_hour() {
    uint64_t hours_since_epoch = time_point_sec(current_time_point()).sec_since_epoch () / 3600;
    
    return hours_since_epoch;
  }
};
