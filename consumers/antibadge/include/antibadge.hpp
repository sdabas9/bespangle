#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT antibadge : public contract {
  public:
    using contract::contract;

    ACTION create(name antibadge, name counterbadge);
    ACTION issue(name antibadge, name to);

  private:
    TABLE messages {
      name    user;
      string  text;
      auto primary_key() const { return user.value; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
};
