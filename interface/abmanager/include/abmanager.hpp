#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ANTIBADGE_CONTRACT "antibadgexxx"
#define ORG_CHECKS_CONTRACT_NAME "interface111"
#define ORCHESTRATOR_CONTRACT "orchestrator"
#define ANTIBADGE_VALIDATION_CONTRACT "antibadgeval"

CONTRACT abmanager : public contract {
  public:
    using contract::contract;

    ACTION create(name org, name authorized,
                  name antibadge,
                  name badge,
                  name type,
                  string offchain_lookup_data,
                  string onchain_lookup_data,
                  vector<name> consumers,
                  string memo);

    ACTION createinv(name org, name authorized,
                     name antibadge,
                     name badge,
                     string offchain_lookup_data,
                     string onchain_lookup_data,
                     string memo);

    ACTION issue(name org, name authorized,
                 name to,
                 name antibadge,
                 uint64_t amount,
                 string memo);

  private:

    TABLE checks {
      name org;
      name checks_contract;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("checks"), checks> checks_table;

    void notify_checks_contract(name org) {
      checks_table _checks( name(ORG_CHECKS_CONTRACT_NAME), get_self().value );
      auto itr = _checks.find(org.value);
      if(itr != _checks.end()) {
        require_recipient(itr->checks_contract);
      }
    }
    
    struct create_args {
        name org;
        name antibadge;
        name badge;
        name type;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct createinv_args {
        name org;
        name antibadge;
        name badge;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct issue_args {
        name org;
        name to;
        name antibadge;
        uint64_t amount;
        string memo;
    };

    struct addfeature_args {
      name org;
      name badge_name;
      name notify_account;
      string memo;
    };
};
