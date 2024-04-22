#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT "simplebadgez"
#define ORG_CONTRACT "orgzzzzzzzzz"
#define SIMPLE_VALIDATION_CONTRACT "simplevalzzz"
#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define STATISTICS_CONTRACT "statisticszz"

#define CUMULATIVE_CONTRACT "cumulativezz"
#define STATISTICS_CONTRACT "statisticszz"
#define ANDEMITTER_CONTRACT "andemitterzz"
#define BOUNDED_AGG_CONTRACT "baggzzzzzzzz"
#define BOUNDED_STATS_CONTRACT "bstatszzzzzz"

CONTRACT simmanager : public contract {
  public:
    using contract::contract;

    ACTION initsimple (name authorized, 
      name org, 
      name badge,
      string offchain_lookup_data, 
      string onchain_lookup_data,
      bool lifetime_aggregate,
      bool lifetime_stats,
      bool emit_secondary_token,
      bool bounded_aggregate,
      bool bounded_stats,
      vector<name> other_consumers,
      string memo);
      
    ACTION givesimple (name authorized,
     name org,
     name badge,
     uint64_t amount,
     name to, 
     string memo );

    ACTION simplebatch (name authorized,
      name org, 
      name badge,
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
    

        // Define the structure of the table
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

    string get_org_code(name org, string failure_identifier) {
      orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
      auto iterator = orgcodes.find(org.value);
      check(iterator != orgcodes.end(), failure_identifier + "Organization not found");
      return iterator->org_code.to_string();
    }

    void validate_badge(name badge, string failure_identifier) {
      string badge_str = badge.to_string();
      check(badge_str.length() == 3, failure_identifier + "badge code must be exactly 3 characters long");
      for (char c : badge_str) {
        check(std::isalpha(c), failure_identifier + "badge code can only contain alphabets");
      }
    }

    symbol validate_and_get_symbol(name org, name badge, string failure_identifier) {
      string org_code = get_org_code(org, failure_identifier);
      validate_badge(badge, failure_identifier);
      string symbol_string = org_code + badge.to_string();
      std::transform(symbol_string.begin(), symbol_string.end(), symbol_string.begin(), ::toupper);
      return symbol(symbol_code(symbol_string), 0);
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

    struct settings_args {
      symbol badge_symbol;
      uint64_t max_no_of_ranks;
    };
};
