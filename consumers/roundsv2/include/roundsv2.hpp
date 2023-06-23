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
