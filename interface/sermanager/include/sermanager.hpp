#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SERIESBADGE_CONTRACT "seriesbadgex"
#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define ORG_CHECKS_CONTRACT_NAME "interface111"
#define SERIES_VALIDATION_CONTRACT "seriesval111"

CONTRACT sermanager : public contract {
  public:
    using contract::contract;

    ACTION createseries (name org, 
      name authorized, 
      name series);

    ACTION createnext (name org, 
      name authorized, 
      name series, 
      name badge, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      vector<name> consumers,
      string memo);

    ACTION issuelatest (name org,
      name authorized, 
      name series, 
      name to, 
      uint64_t count, 
      string memo);

    ACTION issueany (name org,
      name authorized,
      name series,
      uint64_t seq_id,
      name to,
      uint64_t count,
      string memo);
    
    ACTION serieslbatch (name org, 
      name authorized, 
      name series, 
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

    struct createseries_args {
      name org;
      name series;
    };

    struct createnext_args {
      name org;
      name series;
      name badge;
      string offchain_lookup_data;
      string onchain_lookup_data;
      string memo;
    };

    struct issuelatest_args {
      name org;
      name series;
      name to;
      uint64_t count;
      string memo;
    };

    struct issueany_args {
      name org;
      name series;
      uint64_t seq_id;
      name to;
      uint64_t count;
      string memo;
    };
};
