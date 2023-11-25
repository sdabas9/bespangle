#include <eosio/eosio.hpp>
#include <hyperloglog.hpp>

using namespace std;
using namespace eosio;

#define NOTIFICATION_CONTRACT_NAME "notification"
#define ORCHESTRATOR_CONTRACT_NAME "orchestrator"
#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::notifyachiev"

CONTRACT hllemitter : public contract {
  public:
    using contract::contract;

    // Notify on new badge issuance
    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]]
    void notifyachiev (
      name org, 
      name badge,
      name account, 
      name from,
      uint64_t count,
      string memo,  
      vector<name> notify_accounts
    );

    ACTION newemission(
    name org,
    name badge,
    name sender_uniqueness_badge);

    ACTION issuesimple (name org, name to, name badge, uint64_t amount, string memo);

  private:

    // Table definition moved to private
    TABLE balances {
        uint64_t     id;          // Primary Key
        name         account;     // EOS account name
        name         badge;       // Badge name
        vector<uint8_t> hll;      // HyperLogLog data or any other binary data
        
        // Primary Key
        auto primary_key() const { return id; }

        // Composite Key: combination of account and badge
        uint128_t by_account_badge() const { return (static_cast<uint128_t>(account.value) << 64) | badge.value; }

    };

    typedef multi_index<"balances"_n, balances,
        indexed_by<"accountbadge"_n, const_mem_fun<balances, uint128_t, &balances::by_account_badge>>
    > balances_table;

    TABLE emissions {
        name badge;
        name sender_uniqueness_badge;

        auto primary_key() const { return badge.value; }
    };
    typedef multi_index<"emissions"_n, emissions> emissions_table;

    struct issue_args {
      name org;
      name to;
      name badge; 
      uint64_t amount;
      string memo;
    };

};
