#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "router111111"

CONTRACT userprefs : public contract {
  public:
    using contract::contract;

    ACTION allow(name account, name org);
    ACTION unallow(name account, name org, string memo);
    ACTION checkallow(name org, name account);


  private:
    // scoped by account
    TABLE prefs {
      name org;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("prefs"), prefs> prefs_table;
};
