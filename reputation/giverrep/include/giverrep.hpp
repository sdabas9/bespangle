#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define STATS_CONTRACT_NAME "billing11111"
#define CUMULATIVE_CONTRACT_NAME "billing11111"
#define NOTIFICATION_CONTRACT_NAME "billing11111"
#define ORCHESTRATOR_CONTRACT_NAME "router111111"
#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::notifyachiev"

CONTRACT giverrep : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]]
    void notifyachiev (
      name org, 
      name badge,
      name account, 
      name from,
      uint64_t count,
      string memo,  
      vector<name> notify_accounts
    );

    ACTION newemission(
    name org,
    name badge,
    name source_account_rep_badge,
    name emit_badge)
 ;

    ACTION issuesimple (name org, name to, name badge, uint64_t amount, string memo);
  private:
    TABLE emissions {
      name badge;
      name source_account_rep_badge;
      name emit_badge;
      auto primary_key() const { return badge.value; }
    };
    typedef multi_index<name("emissions"), emissions> emissions_table;

    TABLE achievements {
      uint64_t id;
      name account;
      name badge;
      uint64_t count;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("achievements"), achievements,
    indexed_by<name("accountbadge"), const_mem_fun<achievements, uint128_t, &achievements::acc_badge_key>>
    > achievements_table;

    TABLE keystats {
      name    badge;
      uint64_t  max;
      uint64_t mean;
      uint64_t account_count;
      uint64_t total; 
      auto primary_key() const { return badge.value; }
    };
    typedef multi_index<name("keystats"), keystats> keystats_table;

    struct issue_args {
      name org;
      name to;
      name badge; 
      uint64_t amount;
      string memo;
    };

    uint64_t account_balance(name org, name account, name badge) {
      achievements_table _achievements(name(CUMULATIVE_CONTRACT_NAME), org.value);
      auto account_badge_index = _achievements.get_index<name("accountbadge")>();
      uint128_t account_badge_key = ((uint128_t) account.value) << 64 | badge.value;
      auto account_badge_iterator = account_badge_index.find(account_badge_key);
      if(account_badge_iterator == account_badge_index.end() || 
        account_badge_iterator->account != account || 
        account_badge_iterator->badge != badge) {
        return 0;
      } else {
        return account_badge_iterator->count;
      }
    }
};
