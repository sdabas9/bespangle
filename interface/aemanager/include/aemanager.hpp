#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define ANDEMITTER_VALIDATION_CONTRACT "emittervalzz"
#define ANDEMITTER_CONTRACT "andemitterzz"
#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define ORG_CONTRACT "orgzzzzzzzzz"
#define SIMPLEBADGE_CONTRACT "simplebadgez"

CONTRACT aemanager : public contract {
public:
  using contract::contract;

  struct contract_asset {
    name contract;
    asset emit_asset;
  };

  ACTION newemission(name authorized, name org, name emission_name, vector<asset> emitter_criteria, vector<asset> emit_badges, bool cyclic);
  ACTION activate(name authorized, name org, name emission_name);
  ACTION deactivate(name authorized, name org, name emission_name);

  struct init_args {
    name org;
    name emission_name;
    vector<asset> emitter_criteria;
    vector<contract_asset> emit_assets;
    bool cyclic;
  };

  struct activate_args {
    name org;
    name emission_name;
  };

  struct deactivate_args {
    name org;
    name emission_name;
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

  string get_org_code(name org, string failure_identifier) {
    orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
    auto iterator = orgcodes.find(org.value);
    check(iterator != orgcodes.end(), failure_identifier + "Organization not found");
    return iterator->org_code.to_string();
  }
};
