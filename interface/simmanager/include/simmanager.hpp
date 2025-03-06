#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include "orginterface.hpp"

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT "simplebadgey"
#define ORG_CONTRACT "organizayyyy"
#define SIMPLE_VALIDATION_CONTRACT "simplevalyyy"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define STATISTICS_CONTRACT "statisticsyy"

#define CUMULATIVE_CONTRACT "cumulativeyy"
#define SUBSCRIPTION_CONTRACT "subyyyyyyyyy"


CONTRACT simmanager : public contract {
  public:
    using contract::contract;

    ACTION initsimple (name authorized, 
      symbol badge_symbol,
      string offchain_lookup_data, 
      string onchain_lookup_data,
      bool lifetime_aggregate,
      bool lifetime_stats,
      string memo);
      
    ACTION givesimple (name authorized,
     symbol badge_symbol,
     uint64_t amount,
     name to, 
     string memo );

    ACTION simplebatch (name authorized,
      symbol badge_symbol,
      uint64_t amount,
      vector<name> to, 
      string memo);

  private:
    
    struct addfeature_args {
      name org;
      symbol badge_symbol;
      name notify_account;
      string memo;
    };

    struct createsimple_args {
      name org;
      symbol badge_symbol;
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

    struct issuesimple_args {
      name org;
      asset badge_asset;
      name to;
      string memo;
    };

};
