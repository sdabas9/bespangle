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

   ACTION consumesimple(name requester, vector<name> approvers, name org, name to, asset amount, string memo, string short_reason, string stream_reason);                  
   ACTION approve(name approver, name org, uint64_t request_id, string stream_reason);
   ACTION reject(name approver, name org, uint64_t request_id, string stream_reason);
   ACTION rfe(name approver, name org, uint64_t request_id, string stream_reason);
   ACTION evidence(name org, uint64_t request_id, string stream_reason);
   ACTION cancel(name org, uint64_t request_id, string stream_response);
   ACTION execute(name org, uint64_t request_id);
private:

   TABLE simissue {
      uint64_t request_id;
      name to;
      asset amount;
      string memo;
   }
//
   TABLE request {
      uint64_t request_id;
      name action;
      name requester;
      vector<name, name> approvers;
      name status;
      string short_reason; 
   }

   TABLE sequence {
      uint64_t next_request_id;
   }


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
