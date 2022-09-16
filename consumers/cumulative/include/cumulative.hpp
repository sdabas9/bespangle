#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define BILLING_CONTRACT "billing"
#define NEW_BADGE_ISSUANCE_NOTIFICATION "metadata::notifyachiev"

CONTRACT cumulative : public contract {
  public:
    using contract::contract;
    
    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev (
      name org, 
      name badge_contract, 
      name badge_name,
      name account, 
      name from,
      uint8_t count,
      string memo,
      uint64_t badge_id,  
      vector<name> notify_accounts );

    ACTION dummy();

  private:

    struct ramcredits_arg {
      name org;
      name contract;
      uint64_t bytes;
      string memo;
    };
    
    TABLE achievements {
      uint64_t id;
      name account;
      uint64_t badge_id;
      uint32_t count;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badge_id;
      }
    };
    typedef multi_index<name("achievements"), achievements,
    indexed_by<name("accountbadge"), const_mem_fun<achievements, uint128_t, &achievements::acc_badge_key>>
    > achievements_table;

    void deduct_credit (name org, uint32_t bytes, string memo) {
      action {
        permission_level{get_self(), name("active")},
        name(BILLING_CONTRACT),
        name("ramcredits"),
        ramcredits_arg {
          .org = org,
          .contract = get_self(),
          .bytes = bytes,
          .memo = memo}
      }.send();
    }
};
