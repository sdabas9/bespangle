#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "orchestrator"
#define AUTHORITY_CONTRACT "authorityxxx"

CONTRACT gotchabadge : public contract {
  public:
    using contract::contract;

    ACTION create (symbol badge_symbol, 
      time_point_sec starttime, 
      uint64_t cycle_length, 
      uint8_t supply_per_cycle, 
      string offchain_lookup_data,
      string onchain_lookup_data,
      string memo);
      
    ACTION issue (asset badge_asset
      name from, 
      name to,
      string memo);
    
    ACTION starttime(name org, name badge, time_point_sec new_starttime);

    ACTION cyclelength(name org, name badge, uint64_t new_cycle_length);

    ACTION cyclesupply(name org, name badge, uint8_t new_supply_per_cycle);
  

  private:
   // scoped by org
    TABLE metadata {
      symbol badge_symbol;
      time_point_sec starttime; //0000
      uint64_t cycle_length; // 24 hrs
      time_point_sec last_known_cycle_start;
      time_point_sec last_known_cycle_end;
      uint8_t supply_per_cycle;
      auto primary_key() const {return badge_symbol.code().raw(); }
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

    // scoped by account
    TABLE stats {
      asset badge_asset;
      time_point last_claimed_time;
      auto primary_key() const {return badge_asset.symbol.code().raw(); }
    };
    typedef multi_index<name("stats"), stats> stats_table;

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
      name badge;
      name account;
      name from;
      uint64_t count;
      string memo;
    };

    struct initbadge_args {
      name org;
      name badge_name;
      string offchain_lookup_data; 
      string onchain_lookup_data; 
      string memo;
    }; 

    struct create_args { 
      name org; 
      name badge; 
      time_point_sec starttime; 
      uint64_t cycle_length; 
      uint8_t supply_per_cycle; 
      string offchain_lookup_data;
      string onchain_lookup_data; 
      string memo; 
    };


};
