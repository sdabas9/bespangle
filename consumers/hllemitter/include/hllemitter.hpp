#include <eosio/eosio.hpp>
#include <hyperloglog.hpp>

using namespace std;
using namespace eosio;

#define AUTHORITY_CONTRACT "authorityxxx"
#define SIMPLEBADGE_CONTRACT "simplebadgex"
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

  private:
    TABLE auth {
      uint64_t id;
      name contract;
      name action;
      vector<name> authorized_contracts;

      uint64_t primary_key() const { return id; }
      uint128_t get_secondary_key() const { return combine_names(contract, action); }

      static uint128_t combine_names(const name& a, const name& b) {
          return (uint128_t)a.value << 64 | b.value;
      }
    };

    // Declare the table
    typedef eosio::multi_index<"auth"_n, auth,
        indexed_by<"bycontract"_n, const_mem_fun<auth, uint128_t, &auth::get_secondary_key>>
    > auth_table;

    void check_internal_auth (name action) {
      auth_table _auth(name(AUTHORITY_CONTRACT), name(AUTHORITY_CONTRACT).value);

      // Find the authority entry
      uint128_t secondary_key = (uint128_t)get_self().value << 64 | action.value;
      auto secondary_index = _auth.get_index<"bycontract"_n>();
      auto itr = secondary_index.find(secondary_key);
      if (itr != secondary_index.end() && itr->contract!=get_self() && itr->action!=action) {
          check(false, "No authority found for contract " + get_self().to_string() + " and action " + action.to_string());
      }
      auto authorzied_accounts = itr->authorized_contracts;
      for(auto i = 0 ; i < authorzied_accounts.size(); i++ ) {
        if(has_auth(authorzied_accounts[i])) {
          return;
        }
      }
      check(false, "Account not is authorized list of accounts for action " + action.to_string());
    }

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
