#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "metadata2222"

CONTRACT simplebadge : public contract {
  public:
    using contract::contract;

    ACTION create (name org, 
      name badge, 
      vector<name> parent_badges, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);

    ACTION issue (name org, name to, name badge, string memo );


  private:
    TABLE badge {
      name badge;
      vector<name> parent_badges;
      auto primary_key() const {return badge.value; }
    };
    typedef multi_index<name("badge"), badge> badge_table;

    struct achievement_args {
      name org;
      name badge_contract;
      name badge_name;
      name account;
      name from;
      uint8_t count;
      string memo;
    };

    struct initbadge_args {
      name org; 
      name badge_contract; 
      name badge_name;
      string offchain_lookup_data; 
      string onchain_lookup_data; 
      string memo;
    }; 

};
