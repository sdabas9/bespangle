#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT "simplebadgex"
#define ORG_CHECKS_CONTRACT_NAME "interface111"
#define SIMPLE_VALIDATION_CONTRACT "simplevalxxx"
#define ORCHESTRATOR_CONTRACT "orchestrator"

CONTRACT simmanager : public contract {
  public:
    using contract::contract;

    ACTION initsimple (name org, name creator, 
      name badge, 
      vector<name> parent_badges,
      string offchain_lookup_data, 
      string onchain_lookup_data,
      vector<name> consumers,
      string memo);
      
    ACTION givesimple (name org,
     name badge, 
     name authorizer, 
     name to, 
     string memo );

    ACTION simplebatch (name org, 
      name badge, 
      name authorizer, 
      vector<name> to, 
      string memo);
    

  private:
    TABLE checks {
      name org;
      name checks_contract;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("checks"), checks> checks_table;

    void notify_checks_contract(name org) {
      checks_table _checks( name(ORG_CHECKS_CONTRACT_NAME), get_self().value );
      auto itr = _checks.find(org.value);
      if(itr != _checks.end()) {
        require_recipient(itr->checks_contract);
      }
    }
    
    struct addfeature_args {
      name org;
      name badge_name;
      name notify_account;
      string memo;
    };

    struct createsimple_args {
      name org;
      name badge;
      vector<name> parent_badges;
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

    struct issuesimple_args {
      name org;
      name to;
      name badge;
      uint64_t amount;
      string memo;
    };
};
