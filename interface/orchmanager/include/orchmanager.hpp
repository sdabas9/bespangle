#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define ORG_CONTRACT "organizayyyy"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define ORCHESTRATOR_VALIDATION_CONTRACT "orchvalyyyyy"

CONTRACT orchmanager : public contract {
  public:
    using contract::contract;

  ACTION delfeature (name org,
      name authorized, 
      name badge,
      name consumer,
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
    
    struct delfeature_args {
      symbol badge_symbol;
      name notify_account;
      string memo;
    };
};
