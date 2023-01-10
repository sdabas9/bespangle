#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT dagunlock : public contract {
  public:
    using contract::contract;
    struct badge_contract {
      name badge;
      name issuing_contract;
    };

    struct node {
      badge_contract bc;
      vector<node> nodes;
    };


    ACTION hi(name from, string message);
    ACTION clear();
    ACTION initdag(name dagname, node graph);
    

  private:

    TABLE dag {
      name dagname;
      node graph;
      auto primary_key() const { return dagname.value; }
    };
    typedef multi_index<name("dag"), dag> dag_table;

    TABLE user {
      name user;
      uint64_t posid;
    }

    TABLE messages {
      name    user;
      string  text;
      auto primary_key() const { return user.value; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
};
