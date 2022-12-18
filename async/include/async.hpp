#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORG_INTERFACE_CONTRACT "organization"

CONTRACT async : public contract {
  public:
    using contract::contract;

    ACTION qgivesimpl(name org, name to, name badge, string memo);
    ACTION process(name org, uint8_t records_to_process);
  
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
        row.id = _givesimple.available_primary_key();
        row.to = to;
        row.badge = badge;
        row.memo = memo;
      });
    }

    int process_simple(name org, int process_max) {
      int processed = 0;
      givesimple_table _givesimple(get_self(), org.value);
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

};
