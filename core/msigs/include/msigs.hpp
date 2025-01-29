#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <vector>
#include <eosio/permission.hpp>
#include <algorithm>


using namespace eosio;

class [[eosio::contract]] msigs : public contract {
public:
    using contract::contract;
ACTION customperm(name perm_name, std::vector<name> accounts);
ACTION simissuemsig(
                     name proposal_name, 
                     std::vector<name> actors, 
                     symbol badge_symbol, 
                     uint64_t amount, 
                     name to, 
                     uint32_t expiry_duration, 
                     std::string memo);

private:
   struct permission_level_weight {
      permission_level  permission;
      uint16_t          weight;

   };

   struct key_weight {
      eosio::public_key  key;
      uint16_t           weight;


   };

   struct wait_weight {
      uint32_t           wait_sec;
      uint16_t           weight;


   };

   struct authority {
      uint32_t                              threshold = 0;
      std::vector<key_weight>               keys;
      std::vector<permission_level_weight>  accounts;
      std::vector<wait_weight>              waits;

   };
       struct auth {
        name account;
        name permission_name;
        name parent;
        authority auth_data;

    };
};
