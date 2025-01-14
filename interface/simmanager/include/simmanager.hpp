#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT "simplebadgey"
#define ORG_CONTRACT "organizayyyy"
#define SIMPLE_VALIDATION_CONTRACT "simplevalyyy"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define STATISTICS_CONTRACT "statisticsyy"

#define CUMULATIVE_CONTRACT "cumulativeyy"
#define SUBSCRIPTION_CONTRACT "subsyyyyyyyy"


CONTRACT simmanager : public contract {
  public:
    using contract::contract;

    ACTION initsimple (name authorized, 
      symbol badge_symbol,
      string offchain_lookup_data, 
      string onchain_lookup_data,
      bool lifetime_aggregate,
      bool lifetime_stats,
      string memo);
      
    ACTION givesimple (name authorized,
     symbol badge_symbol,
     uint64_t amount,
     name to, 
     string memo );

    ACTION simplebatch (name authorized,
      symbol badge_symbol,
      uint64_t amount,
      vector<name> to, 
      string memo);

  private:
    TABLE checks {
      name org;
      name checks_contract;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("checks"), checks> checks_table;

    void notify_checks_contract(name org) {
      checks_table _checks( name(ORG_CONTRACT), name(ORG_CONTRACT).value );
      auto itr = _checks.find(org.value);
      if(itr != _checks.end()) {
        require_recipient(itr->checks_contract);
      }
    }
    

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


    name get_name_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
        string _symbol_str = _symbol.code().to_string(); // Convert symbol to string
        check(_symbol_str.size() == 7, failure_identifier + "Symbol must have at least 7 characters.");

        // Extract the first 4 characters as org_code
        string _str = _symbol_str.substr(4, 7);

        for (auto & c: _str) {
            c = tolower(c);
        }
        return name(_str);
    }

    name get_org_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
      string _symbol_str = _symbol.code().to_string(); // Convert symbol to string
      check(_symbol_str.size() >= 4, failure_identifier + "symbol must have at least 4 characters.");

      // Extract the first 4 characters as org_code
      string org_code_str = _symbol_str.substr(0, 4);

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
    
    struct addfeature_args {
      name org;
      symbol badge_symbol;
      name notify_account;
      string memo;
    };

    struct createsimple_args {
      name org;
      symbol badge_symbol;
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

    struct issuesimple_args {
      name org;
      asset badge_asset;
      name to;
      string memo;
    };

};
