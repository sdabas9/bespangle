#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORG_INTERFACE_CONTRACT "orgint.rep"

#define ORG_INTERFACE_CREATE_SERIES_NOTIFICATION ORG_INTERFACE_CONTRACT"::createseries" 
#define ORG_INTERFACE_CREATE_NEXT_NOTIFICATION ORG_INTERFACE_CONTRACT"::createnext" 
#define ORG_INTERFACE_ISSUE_LATEST_NOTIFICATION ORG_INTERFACE_CONTRACT"::issuelatest" 
#define ORG_INTERFACE_ISSUE_ANY_NOTIFICATION ORG_INTERFACE_CONTRACT"::issueany" 
#define ORG_INTERFACE_ISSUE_BATCH_NOTIFICATION ORG_INTERFACE_CONTRACT"::serieslbatch" 

CONTRACT serval : public contract {
  public:
    using contract::contract;

   [[eosio::on_notify(ORG_INTERFACE_CREATE_SERIES_NOTIFICATION)]] void series (name org, 
      name authorized, 
      name series);

    [[eosio::on_notify(ORG_INTERFACE_CREATE_NEXT_NOTIFICATION)]]  void next (name org, 
      name authorized, 
      name series, 
      name badge, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      vector<name> consumers,
      string memo);

    [[eosio::on_notify(ORG_INTERFACE_ISSUE_LATEST_NOTIFICATION)]]  void latest (name org,
      name authorized, 
      name series, 
      name to, 
      uint64_t count, 
      string memo);

    [[eosio::on_notify(ORG_INTERFACE_ISSUE_ANY_NOTIFICATION)]]  void any (name org,
      name authorized,
      name series,
      uint64_t seq_id,
      name to,
      uint64_t count,
      string memo);
    
    [[eosio::on_notify(ORG_INTERFACE_ISSUE_BATCH_NOTIFICATION)]]  void batch (name org, 
      name authorized, 
      name series, 
      vector<name> to, 
      string memo);

     ACTION addauth (name org, name action, name authorized_account); 

     ACTION removeauth (name org, name action, name authorized_account);

  private:
    TABLE actionauths {
      name  action;
      vector<name>  authorized_accounts;
      auto primary_key() const { return action.value; }
    };
    typedef multi_index<name("actionauths"), actionauths> actionauths_table;

    void action_authority_check (name org, name action, name account) {
      actionauths_table _actionauths_table (get_self(), org.value);
      auto itr = _actionauths_table.require_find(action.value, "No authority for this action");
      auto authorized_accounts = itr->authorized_accounts;
      for(auto i = 0; i < authorized_accounts.size(); i++) {
        if(authorized_accounts[i] == account) {
          return;
        }
      }
      check(false, "Account not in authorized vector");
    }
};
