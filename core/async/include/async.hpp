#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT async : public contract {
  public:
    using contract::contract;

    ACTION hi(name from, string message);
    ACTION clear();

  private:

    TABLE deferred {
        uint64_t id;
        uint64_t hour_number;
        name contract;
        name from;
        name to;
        name badge;
        uint64_t amount;
        string memo;
        uint64_t created_at;
    }
    TABLE messages {
      name    user;
      string  text;
      auto primary_key() const { return user.value; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
};
