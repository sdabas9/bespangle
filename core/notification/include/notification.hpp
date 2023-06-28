#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORG_INTERFACE_CONTRACT_NAME "orgint.rep"
#define ANDEMITTER_CONTRACT_NAME "andemitter11"
#define SIMPLEBADGE_CONTRACT_NAME "simplep.rep"
#define CLAIMASSET_CONTRACT_NAME "claimasset11"
#define ORCHESTRATOR_CONTRACT_NAME "meta.rep"
#define SERIESBADGE_CONTRACT_NAME "seriesp.rep"

#define ORG_INTERFACE_SERIES_CREATE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ncreateserie"
#define ORG_INTERFACE_SERIES_BADGE_CREATE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ncreatenex"
#define ORG_INTERFACE_SERIES_ISSUE_LATEST_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::nissuelates"
#define ORG_INTERFACE_SERIES_ISSUE_ANY_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::nissuean"

// initsimple notifications
#define ORG_INTERFACE_SIMPLE_CREATE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ninitsimpl"

// simple issue notifications
#define ORG_INTERFACE_SIMPLE_ISSUE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ngivesimpl"
#define ANDEMITTER_SIMPLE_ISSUE_NOTIFICATION ANDEMITTER_CONTRACT_NAME"::givesimple"

// add claimer notification
#define ANDEMITTER_ADD_CLAIMER_NOTIFICATION ANDEMITTER_CONTRACT_NAME"::addclaimer"
#define ORG_INTERFACE_ADD_CLAIMER_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::naddclaime"

// create claimasset notification
#define ORG_INTERFACE_CREATE_CLAIM_ASSET_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::ninitcasse"

// claim claimasset 
#define ORG_INTERFACE_CLAIM_ASSET_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::nclaimasse"

// add feature notification
#define ORG_INTERFACE_ADD_FEATURE_NOTIFICATION ORG_INTERFACE_CONTRACT_NAME"::naddfeatur" 

CONTRACT notification : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(ORG_INTERFACE_SIMPLE_CREATE_NOTIFICATION)]] void ainitsimple (name org, 
      name badge, 
      vector<name> parent_badges, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);
    
    ACTION initsimple(name org, 
      name badge, 
      vector<name> parent_badges, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);



    [[eosio::on_notify(ORG_INTERFACE_SIMPLE_ISSUE_NOTIFICATION)]] void asimpleissue (name org, 
      name to, 
      name badge,
      uint64_t amount,
      string memo);
      
    [[eosio::on_notify(ANDEMITTER_SIMPLE_ISSUE_NOTIFICATION)]] void bsimpleissue (name org, 
      name to, 
      name badge,
      uint64_t amount, 
      string memo);
    
    ACTION givesimple(name org, name to, name badge, uint64_t amount, string memo );


    
    [[eosio::on_notify(ANDEMITTER_ADD_CLAIMER_NOTIFICATION)]] void aaddclaimer (name org, 
      name account, 
      name assetname, 
      uint64_t account_cap,
      string memo);

    [[eosio::on_notify(ORG_INTERFACE_ADD_CLAIMER_NOTIFICATION)]] void baddclaimer(name org, 
      name account, 
      name assetname, 
      uint64_t account_cap, 
      string memo);

    ACTION addclaimer(name org, 
      name account, 
      name assetname, 
      uint64_t account_cap, 
      string memo);


    
    [[eosio::on_notify(ORG_INTERFACE_CREATE_CLAIM_ASSET_NOTIFICATION)]] void ainitclaim(name org, 
      name assetname,
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);
    
    ACTION initcasset(name org, 
      name assetname,
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo);



    [[eosio::on_notify(ORG_INTERFACE_CLAIM_ASSET_NOTIFICATION)]] void aclaimasset(name org, 
      name to, 
      name assetname, 
      string memo );

    ACTION claimasset(name org, 
      name to, 
      name assetname, 
      string memo );



    [[eosio::on_notify(ORG_INTERFACE_ADD_FEATURE_NOTIFICATION)]] void aaddfeature (name org,
      name badge_name,
      name notify_account,
      string memo);

    ACTION addfeature(name org,
      name badge_name,
      name notify_account,
      string memo);





    [[eosio::on_notify(ORG_INTERFACE_SERIES_CREATE_NOTIFICATION)]] void acreateserie(name org, name series);
    ACTION createseries(name org, name series);

    [[eosio::on_notify(ORG_INTERFACE_SERIES_BADGE_CREATE_NOTIFICATION)]] void acreatenex(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo);
    ACTION createnext(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo);

    [[eosio::on_notify(ORG_INTERFACE_SERIES_ISSUE_LATEST_NOTIFICATION)]] void aissuelates(name org, name series, name to, uint64_t count, string memo);
    ACTION issuelatest(name org, name series, name to, uint64_t count, string memo);

    [[eosio::on_notify(ORG_INTERFACE_SERIES_ISSUE_ANY_NOTIFICATION)]] void aissuean(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo);
    ACTION issueany(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo);


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
      uint64_t amount;
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
      string memo;
    };

    struct addfeature_args {
      name org;
      name badge_name;
      name notify_account;
      string memo;
    };

    struct claimasset_args {
      name org;
      name to;
      name assetname;
      string memo;
    };

    struct createseries_args {
      name org;
      name series;
    };

    struct createnext_args {
      name org;
      name series;
      name badge;
      string offchain_lookup_data;
      string onchain_lookup_data;
      string memo;
    };

    struct issuelatest_args {
      name org;
      name series;
      name to;
      uint64_t count;
      string memo;
    };

    struct issueany_args {
      name org;
      name series;
      uint64_t seq_id;
      name to;
      uint64_t count;
      string memo;
    };
};


