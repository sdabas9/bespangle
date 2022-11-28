#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "metadata2222"
#define ORG_INTERFACE_CONTRACT_NAME "organization"
#define BILLING_CONTRACT "billing11111"

#define SIMPLE_BADGE_CREATE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ninitsimpl"
#define SIMPLE_BADGE_ISSUANCE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ngivesimpl"

CONTRACT simplebadge : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(SIMPLE_BADGE_CREATE_NOTIFICATION)]] void extcreate (name org, 
      name badge, 
      vector<name> parent_badges, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);

    [[eosio::on_notify(SIMPLE_BADGE_ISSUANCE_NOTIFICATION)]] void extissue (name org, 
      name to, 
      name badge, 
      string memo);

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

    struct create_args {
      name org;
      name badge;
      vector<name> parent_badges; 
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

    struct issue_args {
      name org;
      name to;
      name badge; 
      string memo;
    };

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

    struct ramcredits_arg {
      name org;
      name contract;
      uint64_t bytes;
      string memo;
    };
    
    void deduct_credit (name org, uint32_t bytes, string memo) {
      action {
        permission_level{get_self(), name("active")},
        name(BILLING_CONTRACT),
        name("ramcredits"),
        ramcredits_arg {
          .org = org,
          .contract = get_self(),
          .bytes = bytes,
          .memo = memo}
      }.send();
    } 

};
