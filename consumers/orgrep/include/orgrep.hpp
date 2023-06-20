#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT orgrep : public contract {
  public:
    using contract::contract;

    ACTION addmetadata(name org, uint64_t badge_id, name attribute, uint8_t value);
    void receivebadge();

  private:
    TABLE metadata {
      uint64_t id;
      uint64_t badge_id;
      name  attribute;
      uint8_t value;
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

    TABLE repview {
      uint64_t id;
      name account;
      name attribute;
      uint64_t value;
    };

    TABLE stats {
      name key;
      uint64_t value;
    };
};
