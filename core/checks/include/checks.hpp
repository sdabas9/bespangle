#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT_NAME "basicissue11"
#define ORCHESTRATOR_CONTRACT_NAME "router111111"
#define CUMULATIVE_CONTRACT_NAME "accounting11"

CONTRACT checks : public contract {
  public:
    using contract::contract;

    struct sbadges {
      name badge;
      uint64_t count;
    };

    ACTION schecki(name org, name account, vector<sbadges> own, vector<sbadges> not_own);

  private:
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

    uint64_t get_badge_id (name org, name assetname) {
      badge_table _badge(name(ORCHESTRATOR_CONTRACT_NAME), org.value);
      auto badge_index = _badge.get_index<name("badgename")>();
      auto badge_iterator = badge_index.find (assetname.value);
      check(badge_iterator->badge_name == assetname, "asset not found");
      return badge_iterator->badge_id;
    }

    uint64_t account_balance(name org, name account, uint64_t badge_id) {
      achievements_table _achievements(name(CUMULATIVE_CONTRACT_NAME), org.value);
      auto account_badge_index = _achievements.get_index<name("accountbadge")>();
      uint128_t account_badge_key = ((uint128_t) account.value) << 64 | badge_id;
      auto account_badge_iterator = account_badge_index.find(account_badge_key);
      if(account_badge_iterator == account_badge_index.end() || 
        account_badge_iterator->account != account || 
        account_badge_iterator->badge_id != badge_id) {
        return 0;
      } else {
        return account_badge_iterator->count;
      }
    }

    void own_check(name org, name account ,name badge, uint64_t count) {
      uint64_t badge_id = get_badge_id(org,  badge);
      uint64_t balance = account_balance(org, account, badge_id);
      check(balance >= count, "account does not hold enough <badge>. has only <balance>, but needs <count> to pass check");
    }

    void not_own_check(name org, name account ,  name badge, uint64_t count) {
      uint64_t badge_id = get_badge_id(org, badge);
      uint64_t balance = account_balance(org, account, badge_id);
      check(balance < count, "account has balance of <balance> for <badge>. to successfully pass check should hold less than <count>");    
    }
};
