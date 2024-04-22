#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <hyperloglog.hpp>

using namespace std;
using namespace eosio;

#define AUTHORITY_CONTRACT "authorityxxx"
#define SIMPLEBADGE_CONTRACT "simplebadgex"
#define ORCHESTRATOR_CONTRACT "orchestrator"
#define ORG_CONTRACT "orgcontr"
#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"

CONTRACT hllemitter : public contract {
  public:
    using contract::contract;

    // Notify on new badge issuance
    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]]
    void notifyachiev (
      name org,
      asset badge_asset, 
      name from, 
      name to, 
      string memo, 
      vector<name> notify_accounts
    );

    ACTION newemission(
      name org,
      symbol badge_symbol,
      vector<symbol> sender_uniqueness_badge_symbols);
    
    ACTION activate(name org, name emission_name);

    ACTION deactivate(name org, name emission_name);

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


    // Scoped by account
    TABLE balances {
        symbol badge_symbol;
        vector<uint8_t> hll;      // HyperLogLog data or any other binary data
        
        // Primary Key
        auto primary_key() const { return badge_symbol.code().raw(); }

    };

    typedef multi_index<"balances"_n, balances> balances_table;

    // scoped by hllemitter contract
    TABLE emissions {
        symbol badge_symbol;
        vector<asset> sender_uniqueness_badge_assets;
        name status; // init, activate, deactivate

        auto primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<"emissions"_n, emissions> emissions_table;

    // scoped by org contract
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

    void validate_org_badge_symbol(name org, symbol badge_symbol, string failure_identifier) {
        name badge_org = get_org_from_badge_symbol (badge_symbol, failure_identifier);
        check(org == badge_org, failure_identifier + " org not same as in badge_symbol");
    }


    struct issue_args {
      name org;
      asset badge_asset;
      name to;
      string memo;
    };

};
