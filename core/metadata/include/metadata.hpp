#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define BILLING_CONTRACT "billingxxxxx"
#define AUTHORITY_CONTRACT "authorityzzz"
#define ORG_CONTRACT "orgzzzzzzzzz"

CONTRACT metadata : public contract {
public:
  using contract::contract;

  ACTION initbadge(symbol badge_symbol, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo);

  ACTION addfeature(symbol badge_symbol, 
    name notify_account, 
    string memo);

  ACTION addnotify(symbol badge_symbol, 
    name notify_account, 
    string memo, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    uint64_t rarity_counts);

  ACTION delfeature(symbol badge_symbol, 
    name notify_account, 
    string memo);

  ACTION delnotify(symbol badge_symbol, 
    name notify_account, 
    string memo, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    uint64_t rarity_counts);

  ACTION achievement(asset badge_asset, 
    name from, 
    name to, 
    string memo);

  ACTION notifyachiev(asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts);

  ACTION mergeinfo(symbol badge_symbol, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo);

  ACTION ckvalidbadge (symbol badge_symbol);

private:

  struct ramcredits_arg {
    name org;
    name contract;
    uint64_t bytes;
    string memo;
  };

  struct syscredits_arg {
    name org;
  };

  struct notifyachievement_args {
    asset badge_asset;
    name from;
    name to;
    string memo;
    vector<name> notify_accounts;
  };

  struct downstream_notify_args {
    symbol badge_symbol;
    name notify_account;
    string memo;
    string offchain_lookup_data;
    string onchain_lookup_data;
    uint64_t rarity_counts;
  };

  struct checkallow_args {
    name org;
    name account;
  };

  struct local_addfeature_args {
    name org;
    name badge;
    name notify_account;
    string memo;
  };

  TABLE badge {
    symbol badge_symbol;
    vector<name> notify_accounts;
    string offchain_lookup_data;
    string onchain_lookup_data;
    uint64_t rarity_counts;
    auto primary_key() const { return badge_symbol.code().raw(); }
  };
  typedef multi_index<name("badge"), badge> badge_table;

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

