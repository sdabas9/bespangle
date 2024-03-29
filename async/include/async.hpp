#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORG_INTERFACE_CONTRACT "interface111"

CONTRACT async : public contract {
  public:
    using contract::contract;

    ACTION qgivesimpl(name org, name to, name badge, string memo);
    ACTION process(name org, uint8_t records_to_process);
    ACTION isolate(name org);
  
  private:

    struct givesimple_args {
      name org;
      name to;
      name badge;
      string memo;
    };

    TABLE givesimple {
      uint64_t id;
      name to;
      name badge;
      string memo;
      auto primary_key() const { return id; }
    };
    typedef multi_index<name("givesimple"), givesimple> givesimple_table;

    TABLE sequence {
      name action;
      uint64_t last_seq_id;
      auto primary_key() const {return action.value; }
    };
    typedef multi_index<name("sequence"), sequence> sequence_table;

    TABLE fgivesimple {
      uint64_t id;
      uint64_t record_id;
      name to;
      name badge;
      string memo;
      auto primary_key() const { return id; }
    };
    typedef multi_index<name("fgivesimple"), fgivesimple> fgivesimple_table;

    void queue_give_simple (name org, name to, name badge, string memo) {
      givesimple_table _givesimple(get_self(), org.value);
      _givesimple.emplace(get_self(), [&](auto& row) {
        row.id = available_seq_id(org, name("givesimple"));
        row.to = to;
        row.badge = badge;
        row.memo = memo;
      });
    }

    uint64_t available_seq_id (name org, name action) {
      sequence_table _sequence (get_self(), org.value);
      auto itr = _sequence.find(action.value);
      uint64_t available_seq_id; 
      if (itr == _sequence.end()) {
        _sequence.emplace(get_self(), [&](auto& row) {
          available_seq_id = 0;
          row.action = action;
          row.last_seq_id = available_seq_id;
        });
      } else {
        _sequence.modify(itr, get_self(), [&](auto& row) {
          available_seq_id = row.last_seq_id + 1;
          row.last_seq_id = available_seq_id;
        });
      }
      return available_seq_id;
    }

    int process_give_simple(name org, int process_max) {
      int processed = 0;
      givesimple_table _givesimple(get_self(), org.value);
      check(_givesimple.begin() != _givesimple.end(), "No records to process"); 
      for(int i = 0 ; i < process_max; i++) {
        
        auto itr = _givesimple.begin();
        if(itr == _givesimple.end()) {
          break;
        }
        action {
        permission_level{get_self(), name("active")},
        name(ORG_INTERFACE_CONTRACT),
        name("agivesimpl"),
        givesimple_args {
          .org = org,
          .to = itr->to,
          .badge = itr->badge,
          .memo = itr->memo}
        }.send();
        itr = _givesimple.erase(itr);
        processed ++;
      }
      return processed;
    }

    void isolate_give_simple(name org) {
      givesimple_table _givesimple(get_self(), org.value);
      auto givesimple_itr = _givesimple.begin();
      check(givesimple_itr != _givesimple.end(), "No records to isolate");
      
      fgivesimple_table _fgivesimple(get_self(), org.value);
      _fgivesimple.emplace(get_self(), [&](auto& row) {
        row.id = _fgivesimple.available_primary_key();;
        row.record_id = givesimple_itr->id;
        row.to = givesimple_itr->to;
        row.badge = givesimple_itr->badge;
        row.memo = givesimple_itr->memo;
      });

      _givesimple.erase(givesimple_itr);
    }
};
