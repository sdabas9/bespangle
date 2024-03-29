#include <eosio/eosio.hpp>
#include <atomic-interface.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define BILLING_CONTRACT "billingxxxxx"
#define ORCHESTRATOR_CONTRACT_NAME "orchestrator"
#define ATOMIC_ASSETS_CONTRACT "atomicassets"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::notifyachiev"
#define NEW_BADGE_SUBSCRIPTION_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::addnotify"
#define ATOMIC_ASSETS_CREATE_TEMPLATE_NOTIFICATION ATOMIC_ASSETS_CONTRACT"::lognewtempl"
#define ATOMIC_ASSETS_SCHEMA_NAME "schemaschema"


CONTRACT aabadge : public contract {
  public:
    using contract::contract;
    
    struct createtemplate_args {
      name authorized_creator;
      name collection_name;
      name schema_name;
      bool transferable;
      bool burnable;
      uint32_t max_supply;
      ATTRIBUTE_MAP immutable_data;
    };

    struct mintaa_args {
      name authorized_creator;
      name collection_name;
      name schema_name;
      uint32_t template_id;
      name new_asset_owner;
      ATTRIBUTE_MAP immutable_data;
      ATTRIBUTE_MAP mutable_data;
      vector <asset> tokens_to_back;
    };

    struct createcol_args {
      name author;
      name collection_name;
      bool allow_notify;
      vector <name> authorized_accounts;
      vector <name> notify_accounts;
      double market_fee;
      ATTRIBUTE_MAP data;
    };
    
    struct createschema_args {
      name authorized_creator;
      name collection_name;
      name schema_name;
      vector <FORMAT> schema_format;
    };

    ACTION initcoll(name org, name collection_name);

    [[eosio::on_notify(NEW_BADGE_SUBSCRIPTION_NOTIFICATION)]] void notifyinit(
      name org,
      name badge,
      name notify_account,
      string memo,
      string offchain_lookup_data,
      string onchain_lookup_data,
      uint64_t rarity_counts);

    [[eosio::on_notify(ATOMIC_ASSETS_CREATE_TEMPLATE_NOTIFICATION)]] void updatebadge(
      int32_t template_id,
      name authorized_creator,
      name collection_name,
      name schema_name,
      bool transferable,
      bool burnable,
      uint32_t max_supply,
      ATTRIBUTE_MAP immutable_data); 

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev (
      name org, 
      name badge,
      name account, 
      name from,
      uint64_t count,
      string memo,  
      vector<name> notify_accounts);

  private:
    TABLE aatemplate {
      name badge;
      uint32_t  template_id;
      auto primary_key() const { return badge.value; }
    };
    typedef multi_index<name("aatemplate"), aatemplate> aatemplate_table;

    TABLE aacollection {
      name org;
      name collection;
      auto primary_key() const {return org.value;}
      uint64_t collection_key() const {return collection.value;}
    };
    typedef multi_index<name("aacollection"), aacollection,
    indexed_by<name("colkey"), const_mem_fun<aacollection, uint64_t, &aacollection::collection_key>>
    > aacollection_table;
};
