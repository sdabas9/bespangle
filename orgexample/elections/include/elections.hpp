#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define EDEN_ORG_INTERFACE_CONTRACT "org"

CONTRACT elections : public contract {
  public:
    using contract::contract;

    ACTION vote(name voter);
    ACTION initevent(name event, name badge);

  private:
    TABLE events {
      name    event;
      name  badge;
      auto primary_key() const { return event.value; }
    };
    typedef multi_index<name("events"), events> events_table;

    struct givesimple_args {
      name badge;
      name authorizer;
      name to;
      string memo;
    };
};
