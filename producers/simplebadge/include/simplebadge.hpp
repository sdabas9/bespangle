#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include "authorityinterface.hpp"

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define SUBSCRIPTION_CONTRACT "subyyyyyyyyy"

CONTRACT simplebadge : public contract {
  public:
    using contract::contract;

    ACTION create (
      name org,
      symbol badge_symbol,
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);

    ACTION issue (name org, asset badge_asset, name to, string memo );


  private:

    struct achievement_args {
      name org;
      asset badge_asset;
      name from;
      name to;
      string memo;
    };

    struct initbadge_args {
      name org;
      symbol badge_symbol;
      string offchain_lookup_data; 
      string onchain_lookup_data; 
      string memo;
    };

    struct billing_args {
      name org;
      uint8_t actions_used;
    };

};
