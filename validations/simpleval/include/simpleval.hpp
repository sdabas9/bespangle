#include <eosio/eosio.hpp>
#include <vector>

using namespace std;
using namespace eosio;
using std::vector;

#define SIMPLE_INTERFACE_CONTRACT "simmanagerxx"

#define SIMPLE_INTERFACE_CREATE_NOTIFICATION SIMPLE_INTERFACE_CONTRACT"::initsimple" 
#define SIMPLE_INTERFACE_ISSUE_NOTIFICATION SIMPLE_INTERFACE_CONTRACT"::givesimple" 
#define SIMPLE_INTERFACE_ISSUE_BATCH_NOTIFICATION SIMPLE_INTERFACE_CONTRACT"::simplebatch" 


CONTRACT simpleval : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(SIMPLE_INTERFACE_CREATE_NOTIFICATION)]] void initsimple (name org, name creator, 
      name badge, 
      vector<name> parent_badges,
      string offchain_lookup_data, 
      string onchain_lookup_data,
      vector<name> consumers,
      string memo);
      
    [[eosio::on_notify(SIMPLE_INTERFACE_ISSUE_NOTIFICATION)]] void givesimple (name org,
     name badge, 
     name authorizer, 
     name to, 
     string memo );

    [[eosio::on_notify(SIMPLE_INTERFACE_ISSUE_BATCH_NOTIFICATION)]] void simplebatch (name org, 
      name badge, 
      name authorizer, 
      vector<name> to, 
      string memo);

    ACTION addbadgeauth (name org, name action, name badge, name authorized_account);

    ACTION delbadgeauth (name org, name action, name badge, name authorized_account);

    ACTION addactionauth (name org, name action, name authorized_account);

    ACTION delactionauth (name org, name action, name authorized_account);


  private:
    TABLE actionauths {
      name  action;
      vector<name>  authorized_accounts;
      auto primary_key() const { return action.value; }
    };
    typedef multi_index<name("actionauths"), actionauths> actionauths_table;

    TABLE badgeauths {
      uint64_t id;
      name action;
      name badge;
      vector<name> authorized_accounts;

      uint64_t primary_key() const { return id; }
      uint128_t secondary_key() const { return combine_names(action, badge); }

      static uint128_t combine_names(const name& a, const name& b) {
          return (uint128_t{a.value} << 64) | b.value;
      }
    };

    typedef eosio::multi_index<"badgeauths"_n, badgeauths,
        indexed_by<"byactionbadge"_n, const_mem_fun<badgeauths, uint128_t, &badgeauths::secondary_key>>
    > badgeauths_table;

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

    bool has_badge_authority (name org, name action, name badge, name account) {
      badgeauths_table badgeauths(get_self(), org.value);
      auto secondary_index = badgeauths.get_index<"byactionbadge"_n>();
      uint128_t secondary_key = badgeauths::combine_names(action, badge);
      auto itr = secondary_index.find(secondary_key);
      if(itr == secondary_index.end() || itr->action != action || itr->badge != badge) {
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
