#include <boundedstats.hpp>

void boundedstats::notifyachiev (name org,
    name badge,
    name account, 
    name from,
    uint64_t count,
    string memo,
    vector<name> notify_accounts) 
   {
      badgeround_table badgeround(_self, org.value);
      auto badge_index = badgeround.get_index<"badge"_n>();
      auto badge_itr = badge_index.find(badge.value);

      // Iterate over all badgerounds with the specified badge
      while (badge_itr != badge_index.end() && badge_itr->badge == badge) {
         keystats_table keystats(_self, org.value);
         uint64_t badgeround_id = badge_itr->badgeround_id;
         auto keystats_itr = keystats.find(badgeround_id);

         if (keystats_itr == keystats.end()) {
            // If there is no entry in keystats table, insert record
            keystats.emplace(org, [&](auto& row) {
               row.badgeround_id = badgeround_id;
               row.round = badge_itr->round;
               row.badge = badge_itr->badge;
               row.max = count;
               row.account_count = 1;
               row.total = count;
            });
         } else {
            achievements_table _achievements( name(BOUNDED_AGG_CONTRACT_NAME), org.value );
            auto account_badge_index = _achievements.get_index<name("accbadge")>();
            uint128_t account_badge_key = ((uint128_t) account.value) << 64 | badgeround_id;
            auto account_badge_iterator = account_badge_index.find (account_badge_key);

            check(account_badge_iterator !=  account_badge_index.end() 
              && account_badge_iterator->account != account
              && account_badge_iterator->badgeround_id != badgeround_id, "unexpected error while calculating boundedstats");

            uint64_t balance = account_badge_iterator->count;
            uint64_t  max;
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
            total = keystats_itr->total + count;
            keystats.modify(keystats_itr, get_self(), [&](auto& row) {
              row.max = max;
              row.account_count = account_count;
              row.total = total;
            });

         }
         ++badge_itr;
      }
   }


    ACTION boundedstats::bootstrap(name org, name round, name badge, uint64_t max, uint64_t account_count, uint64_t total) {
        require_auth(get_self());

        // Fetch badgeround_id from badgeround table
        badgeround_table _badgeround(_self, org.value);
        auto br_index = _badgeround.get_index<"byroundbadge"_n>();
        auto br_itr = br_index.find(((uint128_t)round.value) << 64 | badge.value);
        check(br_itr != br_index.end() || br_itr->round != round || br_itr->badge != badge, "BadgeRound record not found");
        keystats_table _keystats(_self, org.value);
        auto ks_itr = _keystats.find(br_itr->badgeround_id);
        if(ks_itr == _keystats.end()) {
          _keystats.emplace(get_self(), [&](auto& row) {
              row.badgeround_id = br_itr->badgeround_id;
              row.round = round;
              row.badge = badge;
              row.max = max;
              row.account_count = account_count;
              row.total = total;
          });
        } else {
          _keystats.modify(ks_itr, get_self(), [&](auto& row) {
            row.max = max;
            row.account_count = account_count;
            row.total = total;
          });
        }


    }
