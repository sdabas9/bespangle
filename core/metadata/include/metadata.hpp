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

  TABLE orgcode {
    name org;         // Organization identifier, used as primary key
    name org_code;    // Converted org_code, ensuring uniqueness and specific format

    // Specify the primary key
    auto primary_key() const { return org.value; }

    // Specify a secondary index for org_code to ensure its uniqueness
    uint64_t by_org_code() const { return org_code.value; }
  };

  // Declare the table
  typedef eosio::multi_index<"orgcodes"_n, orgcode,
    eosio::indexed_by<"orgcodeidx"_n, eosio::const_mem_fun<orgcode, uint64_t, &orgcode::by_org_code>>
  > orgcode_index;

  name get_org_from_badge_symbol(const symbol& badge_symbol, string failure_identifier) {
    string badge_symbol_str = badge_symbol.code().to_string(); // Convert symbol to string
    check(badge_symbol_str.size() >= 4, failure_identifier + "Badge symbol must have at least 4 characters.");

    // Extract the first 4 characters as org_code
    string org_code_str = badge_symbol_str.substr(0, 4);

    for (auto & c: org_code_str) {
        c = tolower(c);
    }
    name org_code = name(org_code_str);

    // Set up the orgcode table and find the org_code
    orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
    auto org_code_itr = orgcodes.get_index<"orgcodeidx"_n>().find(org_code.value);

    check(org_code_itr != orgcodes.get_index<"orgcodeidx"_n>().end(), failure_identifier + "Organization code not found.");
    check(org_code_itr->org_code == org_code, failure_identifier + "Organization code not found.");
    // Assuming the org is stored in the same row as the org_code
    return org_code_itr->org; // Return the found organization identifier
  }

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

