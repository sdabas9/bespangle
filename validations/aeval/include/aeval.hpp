#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <vector>

using namespace std;
using namespace eosio;
using std::vector;

#define ANDEMITTER_MANAGER_CONTRACT "aemanagerzzz"

#define ANDEMITTER_MANAGER_NEWEMISSION_NOTIFICATION ANDEMITTER_MANAGER_CONTRACT"::newemission" 
#define ANDEMITTER_MANAGER_ACTIVATE_NOTIFICATION ANDEMITTER_MANAGER_CONTRACT"::activate" 
#define ANDEMITTER_MANAGER_DEACTIVATE_NOTIFICATION ANDEMITTER_MANAGER_CONTRACT"::deactivate" 


CONTRACT aeval : public contract {
  public:
    using contract::contract;

    struct contract_asset {
      name contract;
      asset emit_asset;
    };

    [[eosio::on_notify(ANDEMITTER_MANAGER_NEWEMISSION_NOTIFICATION)]] void newemission(name authorized, 
      name org, 
      name emission_name, 
      vector<asset> emitter_criteria, 
      vector<asset> emit_badges, 
      bool cyclic);
      
    [[eosio::on_notify(ANDEMITTER_MANAGER_ACTIVATE_NOTIFICATION)]] void activate(name authorized, 
      name org, 
      name emission_name);

    [[eosio::on_notify(ANDEMITTER_MANAGER_DEACTIVATE_NOTIFICATION)]] void deactivate(name authorized, 
      name org, 
      name emission_name);

    ACTION addemissauth(name org, name action, name emission, name authorized_account);

    ACTION delemissauth(name org, name action, name emission, name authorized_account);

    ACTION addactionauth (name org, name action, name authorized_account);

    ACTION delactionauth (name org, name action, name authorized_account);


  private:
    TABLE actionauths {
      name  action;
      vector<name>  authorized_accounts;
      auto primary_key() const { return action.value; }
    };
    typedef multi_index<name("actionauths"), actionauths> actionauths_table;

    TABLE emissauths {
      uint64_t id;
      name action;
      name emission;
      vector<name> authorized_accounts;

      uint64_t primary_key() const { return id; }
      uint128_t secondary_key() const { return combine_names(action, emission); }

      static uint128_t combine_names(const name& a, const name& b) {
          return (uint128_t{a.value} << 64) | b.value;
      }
    };

    typedef eosio::multi_index<"emissauths"_n, emissauths,
        indexed_by<"actionemiss"_n, const_mem_fun<emissauths, uint128_t, &emissauths::secondary_key>>
    > emissauths_table;

    bool has_action_authority (name org, name action, name account) {
      actionauths_table _actionauths_table (get_self(), org.value);
      auto itr = _actionauths_table.find(action.value);
      if(itr == _actionauths_table.end()) {
        return false;
      }
      auto authorized_accounts = itr->authorized_accounts;
      for(auto i = 0; i < authorized_accounts.size(); i++) {
        if(authorized_accounts[i] == account) {
          return true;
        }
      }
      return false;
    }

    bool has_emission_authority (name org, name action, name emission, name account) {
      emissauths_table emissauths(get_self(), org.value);
      auto secondary_index = emissauths.get_index<"actionemiss"_n>();
      uint128_t secondary_key = emissauths::combine_names(action, emission);
      auto itr = secondary_index.find(secondary_key);
      if(itr == secondary_index.end() || itr->action != action || itr->emission != emission) {
        return false;
      }
      auto authorized_accounts = itr->authorized_accounts;
      for(auto i = 0; i < authorized_accounts.size(); i++) {
        if(authorized_accounts[i] == account) {
          return true;
        }
      }
      return false;
    }
};
