#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "orchestrator"
#define AUTHORITY_CONTRACT "authorityzzz"

CONTRACT gotchabadge : public contract {
  public:
    using contract::contract;

    ACTION create (name org, 
      name badge, 
      time_point_sec starttime, 
      uint64_t cycle_length, 
      uint8_t supply_per_cycle, 
      string offchain_lookup_data,
      string onchain_lookup_data,
      string memo);
      
    ACTION give (name org, 
      name badge, 
      name from, 
      name to, 
      uint64_t amount, 
      string memo );
    
    ACTION starttime(name org, name badge, time_point_sec new_starttime);

    ACTION cyclelength(name org, name badge, uint64_t new_cycle_length);

    ACTION cyclesupply(name org, name badge, uint8_t new_supply_per_cycle);
  

  private:
   // scoped by org
    TABLE metadata {
      name badge;
      time_point_sec starttime; //0000
      uint64_t cycle_length; // 24 hrs
      time_point_sec last_known_cycle_start;
      time_point_sec last_known_cycle_end;
      uint8_t supply_per_cycle;
      auto primary_key() const {return badge.value; }
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

    // scoped by org
    TABLE stats {
      uint64_t id;
      name account;
      name badge;
      uint8_t balance;
      time_point last_claimed_time;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("stats"), stats,
    indexed_by<name("accountbadge"), const_mem_fun<stats, uint128_t, &stats::acc_badge_key>>
    > stats_table;

    TABLE auth {
      uint64_t id;
      name contract;
      name action;
      vector<name> authorized_contracts;

      uint64_t primary_key() const { return id; }
      uint128_t get_secondary_key() const { return combine_names(contract, action); }

      static uint128_t combine_names(const name& a, const name& b) {
          return (uint128_t)a.value << 64 | b.value;
      }
    };

    // Declare the table
    typedef eosio::multi_index<"auth"_n, auth,
        indexed_by<"bycontract"_n, const_mem_fun<auth, uint128_t, &auth::get_secondary_key>>
    > auth_table;

    void check_internal_auth (name action) {
      auth_table _auth(name(AUTHORITY_CONTRACT), name(AUTHORITY_CONTRACT).value);

      // Find the authority entry
      uint128_t secondary_key = (uint128_t)get_self().value << 64 | action.value;
      auto secondary_index = _auth.get_index<"bycontract"_n>();
      auto itr = secondary_index.find(secondary_key);
      if (itr != secondary_index.end() && itr->contract!=get_self() && itr->action!=action) {
          check(false, "No authority found for contract " + get_self().to_string() + " and action " + action.to_string());
      }
      auto authorzied_accounts = itr->authorized_contracts;
      for(auto i = 0 ; i < authorzied_accounts.size(); i++ ) {
        if(has_auth(authorzied_accounts[i])) {
          return;
        }
      }
      check(false, "Account not is authorized list of accounts for action " + action.to_string());
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
