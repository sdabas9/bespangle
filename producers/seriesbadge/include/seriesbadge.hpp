#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "metadata.rep"

CONTRACT seriesbadge : public contract {
  public:
    using contract::contract;

    ACTION define(name org, name family);
    ACTION createnext(name org, name family, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo);
    ACTION issuelatest(name org, name family, name to, string memo);

  private:
    TABLE metadata {
      name family;
      uint64_t seq_id;
      auto primary_key() const {return family.value; }
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

    TABLE badge {
      uint64_t id;
      name family;
      uint64_t seq_id;
      name badge;
      auto primary_key() const {return id; }

      uint128_t family_seq_id_key() const {
        return ((uint128_t) seq_id) << 64 | family.value;
      }
      uint128_t family_badge_key() const {
        return ((uint128_t) family.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("badge"), badge,
    indexed_by<name("familyseqid"), const_mem_fun<badge, uint128_t, &badge::family_seq_id_key>>,
    indexed_by<name("familybadge"), const_mem_fun<badge, uint128_t, &badge::family_badge_key>>
    > badge_table;


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

    struct isrecognized_args {
      name org;
      name contract;
    };  

};
