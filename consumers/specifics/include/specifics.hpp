#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT specifics : public contract {
  public:
    using contract::contract;

    ACTION expirytime(name account, name badge, uint64_t expiry_time);

  private:
    TABLE expirables {
      uint64_t issuance_id;
      name account;
      name badge;
      uint64_t issuance_time;
      uint64_t expiry_time;  
      auto primary_key() const { return expiry_id; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
};
