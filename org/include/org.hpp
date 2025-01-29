#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SUBSCRIPTION_CONTRACT "subsyyyyyyyy"

CONTRACT org : public contract {
  public:
    using contract::contract;

    ACTION chkscontract (name org, name checks_contract);

    ACTION initorgcode(name org, string org_code);

    ACTION initautocode(name org);

    ACTION nextbadge(name org);

    ACTION nextemission(name org);

    


  private:

    TABLE checks {
      name org;
      name checks_contract;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("checks"), checks> checks_table;

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

    struct haspackage_args {
      name org;
    };

};

