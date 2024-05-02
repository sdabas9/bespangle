#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define GIVER_REP_VALIDATION_CONTRACT "grvalyyyyyyy"
#define GIVER_REP_CONTRACT "giverrepyyyy"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define ORG_CONTRACT "organizayyyy"

CONTRACT grmanager : public contract {
public:
  using contract::contract;

  ACTION newemission(
    name authorized,
    symbol badge_symbol,
    symbol giver_rep_badge_symbol, 
    vector<symbol> emit_badge_symbols);

  ACTION activate(name authorized, symbol badge_symbol);

  ACTION deactivate(name authorized, symbol badge_symbol);

  struct newemission_args {
    name org;
    symbol badge_symbol;
    symbol giver_rep_badge_symbol;
    vector<symbol> emit_badge_symbols;
  };

  struct activate_args {
    name org;
    symbol badge_symbol;
  };

  struct deactivate_args {
    name org;
    symbol badge_symbol;
  };

  struct addfeature_args {
    name org;
    symbol badge_symbol;
    name notify_account;
    string memo;
  };

private:
  TABLE checks {
    name org;
    name checks_contract;

    auto primary_key() const { return org.value; }
  };

  typedef multi_index<name("checks"), checks> checks_table;

  void notify_checks_contract(name org) {
    checks_table _checks(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
    auto itr = _checks.find(org.value);
    if (itr != _checks.end()) {
      require_recipient(itr->checks_contract);
    }
  }

  TABLE orgcode {
    name org; // Organization identifier, used as primary key
    name org_code; // Converted org_code, ensuring uniqueness and specific format

    auto primary_key() const { return org.value; }
    uint64_t by_org_code() const { return org_code.value; }
  };

  typedef eosio::multi_index<"orgcodes"_n, orgcode,
    eosio::indexed_by<"orgcodeidx"_n, eosio::const_mem_fun<orgcode, uint64_t, &orgcode::by_org_code>>
  > orgcode_index;

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
};
