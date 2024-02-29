#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT authority : public contract {
  public:
    using contract::contract;

    ACTION addauth(name contract, name action, name authorized_contract);
    ACTION removeauth(name contract, name action, name authorized_contract);
    ACTION hasauth(name contract, name action, name account);

  private:
    // scoped by contract
    TABLE auth {
      name action;
      vector<name> authorized_contracts;
      uint64_t primary_key() const { return action.value; }
    };
    typedef eosio::multi_index<"auth"_n, auth> auth_table;
};
