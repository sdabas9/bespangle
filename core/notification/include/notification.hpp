#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;



#define ORG_INTERFACE_CONTRACT_NAME "organization"
#define ROLLUP_CONTRACT_NAME "rollup"
#define SIMPLEBADGE_CONTRACT_NAME "sacontract"
#define CLAIMASSET_CONTRACT_NAME "claimasset"

// initsimple notifications
#define ORG_INTERFACE_SIMPLE_CREATE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ninitsimpl"

// simple issue notifications
#define ORG_INTERFACE_SIMPLE_ISSUE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ngivesimpl"
#define ROLLUP_SIMPLE_ISSUE_NOTIFICATION ROLLUP_CONTRACT_NAME"::givesimple"

// add claimer notification
#define ROLLUP_ADD_CLAIMER_NOTIFICATION ROLLUP_CONTRACT_NAME"::initsimple"

// create claimasset notification
#define ORG_INTERFACE_CREATE_CLAIM_ASSET_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::create"



CONTRACT notification : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(ORG_INTERFACE_SIMPLE_CREATE_NOTIFICATION)]] void ainitsimple (name org, 
      name badge, 
      vector<name> parent_badges, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);

    [[eosio::on_notify(ORG_INTERFACE_SIMPLE_ISSUE_NOTIFICATION)]] void asimpleissue (name org, 
      name to, 
      name badge,
      string memo);
      
    [[eosio::on_notify(ROLLUP_SIMPLE_ISSUE_NOTIFICATION)]] void bsimpleissue (name org, 
      name to, 
      name badge, 
      string memo);

    [[eosio::on_notify(ROLLUP_ADD_CLAIMER_NOTIFICATION)]] void aaddclaimer (name org, 
      name account, 
      name assetname, 
      uint64_t account_cap);

    [[eosio::on_notify(ORG_INTERFACE_CREATE_CLAIM_ASSET_NOTIFICATION)]] void ainitclaim (name org, 
      name assetname, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);

    ACTION initsimple(name org, 
      name badge, 
      vector<name> parent_badges, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);

    ACTION issuesimple(name org, name to, name badge, string memo );

    ACTION addclaimer(name org, name account, name assetname, uint64_t account_cap);

    ACTION initclaim(name org, 
      name assetname,
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);

  private:

    struct create_simplebadge_args {
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

    struct create_claimasset_args {
      name org;
      name assetname;
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

    struct addclaimer_args {
      name org;
      name account;
      name assetname; 
      uint64_t account_cap;
    };
};


