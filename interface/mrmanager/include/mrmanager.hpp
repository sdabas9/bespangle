#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define NOTIFICATION_CONTRACT "notification"
#define MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT "mrnotifyxxxx"
#define ORG_CHECKS_CONTRACT_NAME "interface111"

CONTRACT mrmanager : public contract {
  public:
    using contract::contract;

  ACTION naddfeatur (name org,
    name badge_name, 
    name notify_account, 
    string memo);

  ACTION initgotcha (name org,
    name creator, 
    name badge, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string offchain_lookup_data, 
    string onchain_lookup_data,
    vector<name> consumers,
    string memo);

  ACTION ninitgotch (name org, 
    name badge, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string offchain_lookup_data, 
    string onchain_lookup_data,
    vector<name> consumers,
    string memo);


  ACTION givegotcha (name org, name badge, name from, name to, uint8_t amount, string memo );

  ACTION ngivegotch (name org, name badge, name from, name to, uint8_t amount, string memo);

  private:
    TABLE checks {
      name org;
      name checks_contract;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("checks"), checks> checks_table;

    TABLE systemchecks {
      name org;
      vector<name> system_check_contracts;
      auto primary_key() const {return org.value; }
    };
    typedef multi_index<name("systemchecks"), systemchecks> systemchecks_table;

    void notify_checks_contract(name org) {
      checks_table _checks( name(ORG_CHECKS_CONTRACT_NAME), get_self().value );
      auto itr = _checks.find(org.value);
      if(itr != _checks.end()) {
        require_recipient(itr->checks_contract);
      }
    }
    
    void notify_linked_inbuilt_checks_contract(name org) {
      systemchecks_table systemchecks(name(ORG_CHECKS_CONTRACT_NAME), get_self().value);
      auto itr = systemchecks.find(org.value);
      if(itr != systemchecks.end()) {
        for(auto i = 0 ; i < itr->system_check_contracts.size(); i++) {
          require_recipient(itr->system_check_contracts[i]);
        }
      }
    }

    struct addfeature_args {
      name org;
      name badge_name;
      name notify_account;
      string memo;
    };
    
    struct creategotcha_args {
      name org;
      name badge;
      time_point_sec starttime;
      uint64_t cycle_length;
      uint8_t supply_per_cycle;
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

    struct givegotcha_args {
      name org;
      name badge;
      name from;
      name to;
      uint8_t amount;
      string memo;
    };
};
