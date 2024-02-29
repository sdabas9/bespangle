#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#define CUMULATIVE_CONTRACT "cumulativezz"
#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define AUTHORITY_CONTRACT "authorityzzz"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"


using namespace std;
using namespace eosio;

CONTRACT statistics : public contract {
  public:
    using contract::contract;

  ACTION bootstrap(symbol badge_symbol, 
    uint64_t max_issued,
    uint64_t account_count, 
    uint64_t total_issued);

  [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev(
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts);

  private:
    TABLE keystats {
      symbol    badge_symbol;
      uint64_t  max_issued;
      uint64_t account_count;
      uint64_t total_issued; 
      auto primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<name("keystats"), keystats> keystats_table;

    TABLE account {
      asset    balance;
      uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };
    typedef eosio::multi_index<"accounts"_n, account> accounts;

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

};
