#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "router111111"
#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::notifyachiev"
#define NEW_BADGE_SUBSCRIPTION_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::addnotify"

CONTRACT tap : public contract {
  public:
    using contract::contract;


    [[eosio::on_notify(NEW_BADGE_SUBSCRIPTION_NOTIFICATION)]] void notifyinit(
      name org,
      name badge_name,
      name notify_account,
      string memo, 
      uint64_t badge_id, 
      string offchain_lookup_data,
      string onchain_lookup_data,
      uint32_t rarity_counts);

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev (
      name org, 
      name badge_name,
      name account, 
      name from,
      uint64_t count,
      string memo,
      uint64_t badge_id,  
      vector<name> notify_accounts);

    ACTION pause(name org, name assetname);
    ACTION resume(name org, name assetname);
    ACTION timebound(name org, name assetname, time_point_sec start_time, time_point_sec end_time);
    ACTION supplybound(name org, name assetname, uint64_t supplycap);
    ACTION removesb(name org, name assetname);
    ACTION removetb(name org, name assetname);
  private:
    TABLE tapstatus {
      uint64_t badge_id;
      bool  pause;
      bool time_bound;
      time_point start_time;
      time_point end_time;
      bool supply_bound;
      uint64_t supplycap;
      uint64_t current_supply;
      auto primary_key() const { return badge_id; }
    };
    typedef multi_index<name("tapstatus"), tapstatus> tapstatus_table;

    TABLE badge {
      uint64_t badge_id;
      name badge_name;
      vector<name> notify_accounts;
      string offchain_lookup_data;
      string onchain_lookup_data;
      uint64_t rarity_counts;
      auto primary_key() const {return badge_id; }
      uint64_t badge_key() const {
        return badge_name.value;
      }
    };
    typedef multi_index<name("badge"), badge,
    indexed_by<name("badgename"), const_mem_fun<badge, uint64_t, &badge::badge_key>>    
    > badge_table;

    uint64_t get_badge_id (name org, name assetname) {
      badge_table _badge(name(ORCHESTRATOR_CONTRACT_NAME), org.value);
      auto badge_index = _badge.get_index<name("badgename")>();
      auto badge_iterator = badge_index.find (assetname.value);
      check(badge_iterator->badge_name == assetname, "asset not found");
      return badge_iterator->badge_id;
    }
};
