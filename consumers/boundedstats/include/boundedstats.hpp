#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "router111111"
#define BOUNDED_AGG_CONTRACT_NAME "dasfd"
#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::notifyachiev"

CONTRACT boundedstats : public contract {
  public:
    using contract::contract;

    ACTION bootstrap(name org,
      name round,
      name badge, 
      uint64_t max, 
      uint64_t account_count, 
      uint64_t total);

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev (name org,
    name badge,
    name account, 
    name from,
    uint64_t count,
    string memo,
    vector<name> notify_accounts);

  private:
    TABLE keystats {
        uint64_t badgeround_id;
        name round;
        name badge;
        uint64_t max;
        uint64_t account_count;
        uint64_t total;

        uint64_t primary_key() const { return badgeround_id; }
    };
    typedef multi_index<"keystats"_n, keystats> keystats_table;

    TABLE badgeround {
        uint64_t badgeround_id;
        name round;
        name badge;
        name status;
        uint64_t primary_key() const { return badgeround_id; }
        uint128_t round_badge_key() const {
            return ((uint128_t) round.value) << 64 | badge.value;
        }
        uint64_t get_badge() const { return badge.value; }
    };
    typedef eosio::multi_index<"badgeround"_n, badgeround,
        indexed_by<"byroundbadge"_n, const_mem_fun<badgeround, uint128_t, &badgeround::round_badge_key>>,
        indexed_by<"badge"_n, const_mem_fun<badgeround, uint64_t, &badgeround::get_badge>>
    > badgeround_table;

    TABLE achievements {
      uint64_t id;
      name account;
      uint64_t badgeround_id;
      uint64_t count;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badgeround_id;
      }
    };
    typedef eosio::multi_index<"achievements"_n, achievements,
        indexed_by<"accbadge"_n, const_mem_fun<achievements, uint128_t, &achievements::acc_badge_key>>
    > achievements_table;
};
