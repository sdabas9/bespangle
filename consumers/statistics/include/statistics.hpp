#include <eosio/eosio.hpp>


#define CUMULATIVE_CONTRACT_NAME "accounting11"
#define ORCHESTRATOR_CONTRACT_NAME "router111111"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::notifyachiev"


using namespace std;
using namespace eosio;

CONTRACT statistics : public contract {
  public:
    using contract::contract;

    ACTION bootstrap(name org,
      name badge, 
      uint64_t max, 
      uint64_t account_count, 
      uint64_t total);

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev (
      name org, 
      name badge_name,
      name account, 
      name from,
      uint64_t count,
      string memo,
      uint64_t badge_id,  
      vector<name> notify_accounts);

  private:
    TABLE keystats {
      name    badge;
      uint64_t  max;
      uint64_t account_count;
      uint64_t total; 
      auto primary_key() const { return badge.value; }
    };
    typedef multi_index<name("keystats"), keystats> keystats_table;

    TABLE achievements {
      uint64_t id;
      name account;
      uint64_t badge_id;
      uint64_t count;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badge_id;
      }
    };
    typedef multi_index<name("achievements"), achievements,
    indexed_by<name("accountbadge"), const_mem_fun<achievements, uint128_t, &achievements::acc_badge_key>>
    > achievements_table;
};
