#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#include "orginterface.hpp"
#include "authorityinterface.hpp"

using namespace std;
using namespace eosio;

#define BILLING_CONTRACT "billingxxxxx"

CONTRACT metadata : public contract {
public:
  using contract::contract;

  ACTION initbadge(
    name org,
    symbol badge_symbol, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo);

  ACTION addfeature(
    name org,
    symbol badge_symbol, 
    name notify_account, 
    string memo);

  ACTION addnotify(
    name org,
    symbol badge_symbol, 
    name notify_account, 
    string memo, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    uint64_t rarity_counts);

  ACTION delfeature(
    name org,
    symbol badge_symbol, 
    name notify_account, 
    string memo);

  ACTION delnotify(
    name org,
    symbol badge_symbol, 
    name notify_account, 
    string memo, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    uint64_t rarity_counts);

  ACTION achievement(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo);

  ACTION notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts);

  ACTION mergeinfo(
    name org,
    symbol badge_symbol, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo);

  ACTION nextbadge(name org);
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
    name org;
    asset badge_asset;
    name from;
    name to;
    string memo;
    vector<name> notify_accounts;
  };

  struct downstream_notify_args {
    name org;
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


  // Table to store the next badge code
  TABLE autocode {
      name org;             // Organization
      symbol last_auto_symbol; // Next badge code

      uint64_t primary_key() const { return org.value; }
  };
  typedef multi_index<"autocode"_n, autocode> autocode_table;

  // Helper function to increment auto codes
  string increment_auto_code(const string& code) {
      string next_code = code;
      for (int i = code.size() - 1; i >= 0; --i) {
          if (next_code[i] == 'z') {
              next_code[i] = 'a';
          } else {
              next_code[i]++;
              break;
          }
      }
      return next_code;
  }

  // Helper function to check if an auto code exists
  bool autocode_exists(const string& badge_symbol) {
      badge_table badges(get_self(), get_self().value);
      return badges.find(symbol_code(badge_symbol).raw()) != badges.end();
  }


};

