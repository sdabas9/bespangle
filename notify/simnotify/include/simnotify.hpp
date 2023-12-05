#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SIMPLE_INTERFACE_CONTRACT_NAME "simmanagerxx"
#define ANDEMITTER_CONTRACT_NAME "andemitterxx"
#define SIMPLEBADGE_CONTRACT_NAME "simplebadgex"
// initsimple notifications
#define INTERFACE_SIMPLE_CREATE_NOTIFICATION SIMPLE_INTERFACE_CONTRACT_NAME"::ninitsimpl"

// simple issue notifications
#define INTERFACE_SIMPLE_ISSUE_NOTIFICATION SIMPLE_INTERFACE_CONTRACT_NAME"::ngivesimpl"
#define ANDEMITTER_SIMPLE_ISSUE_NOTIFICATION ANDEMITTER_CONTRACT_NAME"::givesimple"

CONTRACT simnotify : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(INTERFACE_SIMPLE_CREATE_NOTIFICATION)]] void ainitsimple (name org, 
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



    [[eosio::on_notify(INTERFACE_SIMPLE_ISSUE_NOTIFICATION)]] void asimpleissue (name org, 
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
};
