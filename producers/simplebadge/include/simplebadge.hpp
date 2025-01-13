#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define AUTHORITY_CONTRACT "authorityyyy"
#define SUBSCRIPTION_CONTRACT "subsyyyyyyyy"

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
    // scoped by contract
    TABLE auth {
      name action;
      vector<name> authorized_contracts;
      uint64_t primary_key() const { return action.value; }
    };
    typedef eosio::multi_index<"auth"_n, auth> auth_table;

    void check_internal_auth (name action, string failure_identifier) {
      auth_table _auth(name(AUTHORITY_CONTRACT), _self.value);
      auto itr = _auth.find(action.value);
      check(itr != _auth.end(), failure_identifier + "no entry in authority table for this action and contract");
      auto authorized_contracts = itr->authorized_contracts;
      for(auto i = 0 ; i < authorized_contracts.size(); i++ ) {
        if(has_auth(authorized_contracts[i])) {
          return;
        }
      }
      check(false, failure_identifier + "Calling contract not in authorized list of accounts for action " + action.to_string());
    }

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
