#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT rewards : public contract {
  public:
    using contract::contract;

    ACTION test(name fn, name sn);

  private:
    TABLE messages {
      name token;
      vector
      name    token;
      
      string  text;
      auto primary_key() const { return user.value; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
};
