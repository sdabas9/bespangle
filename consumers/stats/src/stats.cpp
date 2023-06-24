#include <stats.hpp>


void stats::notifyachiev (
      name org, 
      name badge_name,
      name account, 
      name from,
      uint64_t count,
      string memo,
      uint64_t badge_id,  
      vector<name> notify_accounts) {

    keystats_table _keystats( _self, org.value );  
    auto keystats_itr = _keystats.find(badge_name.value);


    if(keystats_itr == _keystats.end()) {
      _keystats.emplace(get_self(), [&](auto& row){
        row.badge = badge_name;
        row.max = count;
        row.mean = count;
        row.account_count = 1;
        row.total = row.total + count;
      });
    } else {

      achievements_table _achievements( name(CUMULATIVE_CONTRACT_NAME), org.value );
      auto account_badge_index = _achievements.get_index<name("accountbadge")>();
      uint128_t account_badge_key = ((uint128_t) account.value) << 64 | badge_id;
      auto account_badge_iterator = account_badge_index.find (account_badge_key);

      if(account_badge_iterator == account_badge_index.end() || account_badge_iterator->account != account || account_badge_iterator->badge_id!=badge_id) {
        check(false, "possibly cumulative consumer is not followed by stats consumer");
      } else {
        uint64_t balance = account_badge_iterator->count;
        uint64_t  max;
        uint64_t mean;
        uint64_t account_count;
        uint64_t total; 
        if(balance > keystats_itr->max) {
          max = balance;
        } else {
          max = keystats_itr->max;
        }
        if(balance == count) {
          account_count = keystats_itr->account_count + 1;
        } else {
          account_count = keystats_itr->account_count;
        }
        total = total + count;
        mean = total / account_count;

        _keystats.modify(keystats_itr, get_self(), [&](auto& row) {
          row.max = max;
          row.mean = mean;
          row.account_count = account_count;
          row.total = total;
        });
      }

    }
}

ACTION stats::bootstrap(name org,
      name badge, 
      uint64_t max, 
      uint64_t mean,
      uint64_t account_count, 
      uint64_t total) {
    require_auth(get_self());
    keystats_table _keystats( _self, org.value );  
    auto keystats_itr = _keystats.find(badge.value);

    if(keystats_itr == _keystats.end()) {
      _keystats.emplace(get_self(), [&](auto& row){
        row.badge = badge;
        row.max = max;
        row.mean = mean;
        row.account_count = account_count;
        row.total = row.total;
      });
    } else {
      _keystats.modify(keystats_itr, get_self(), [&](auto& row) {
        row.max = max;
        row.mean = mean;
        row.account_count = account_count;
        row.total = total;
      });
    }

}

