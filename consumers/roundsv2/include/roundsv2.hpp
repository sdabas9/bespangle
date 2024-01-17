#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

// round start
// rounds ep 
// scorecard
// reward

CONTRACT roundsv2 : public contract {
  public:
    using contract::contract;

    ACTION hi(name from, string message);
    ACTION clear();

  private:
    // for a round there can be a scorecard.
    // for a scorecard there can be rewards.
    // rankreward is one type of reward.

    TABLE rounds {
      name round;
      string desc;
      name status;
      auto primary_key() const {return round.value; }
    };
    typedef multi_index<name("rounds"), rounds> rounds_table;

    TABLE roundbadge {
      uint64_t roundbadge_id;
      name round;
      name badge;
    };
    typedef multi_index<name("roundbadge"), roundbadge> roundbadge_table;

    TABLE achievements {
      uint64_t id;
      name account;
      uint64_t roundbadge_id;
      uint64_t count;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badge_id;
      }
    };
    typedef multi_index<name("achievements"), achievements,
    indexed_by<name("accountbadge"), const_mem_fun<achievements, uint128_t, &achievements::acc_badge_key>>
    > achievements_table;

    TABLE rounds {
      name    round;
      name   account;
      name badge;
      uint64_t balance;
    };
    typedef multi_index<name("messages"), messages> messages_table;

    TABLE scoremeta {
      name round;
      name score_badge;
      uint64_t value;
      vector<name> open_rewards;
      vector<name> settled_rewards;
    };

    TABLE rankreward {
      name round;
      uint64_t rank;
      map<> assets;
    };

    TABLE scorecard {
      name score_badge;
      name account;
      uint64_t balance;
      vector<name> open_rewards;
      vector<name> settled_rewards;
    };
    typedef multi_index<name("messages"), messages> messages_table;

    TABLE scorestats {
      name score_badge;
      name stat;
      uint64_t value;

    };
};
