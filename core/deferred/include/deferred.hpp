#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

CONTRACT deferred : public contract {
public:
  using contract::contract;

  ACTION addrecord(name org, name contract, name from, name to, uint64_t amount, std::string memo, name badge, time_point_sec execution_time);
  ACTION execute(name org, uint64_t record_id);
  ACTION execbyhour(name org, uint64_t hour, uint64_t step);

  struct issue_args {
    name org;
    name badge;
    name from;
    name to;
    uint64_t amount;
    std::string memo;
  };


private:
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
  > records_table;

  uint64_t get_hour_since_epoch(const time_point_sec& execution_time) {
    const uint64_t hour_since_epoch = execution_time.sec_since_epoch () / 3600;
    return hour_since_epoch;
  }

  uint64_t get_current_hour() {
    uint64_t hours_since_epoch = time_point_sec(current_time_point()).sec_since_epoch () / 3600;
    
    return hours_since_epoch;
  }
};
