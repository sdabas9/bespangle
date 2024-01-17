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
    TABLE auth {
        uint64_t id;
        name contract;
        name action;
        vector<name> authorized_contracts;

        uint64_t primary_key() const { return id; }
        uint128_t get_secondary_key() const { return combine_names(contract, action); }

        static uint128_t combine_names(const name& a, const name& b) {
            return (uint128_t)a.value << 64 | b.value;
        }
    };

    // Declare the table
    typedef eosio::multi_index<"auth"_n, auth,
        indexed_by<"bycontract"_n, const_mem_fun<auth, uint128_t, &auth::get_secondary_key>>
    > auth_table;
};
