#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define BYTES_PER_CREDIT "bytespercr"
#define RAM_IN_BYTES_PER_SYS_TOKEN "ramrate"
#define MAX_CREDIT_BALANCE "maxcredits"

#define AUTHORITY_CONTRACT "authorityzzz"

CONTRACT orgbill : public contract {
  public:
    using contract::contract;
    struct notify_args {
      name org;
      uint32_t total_credits;
      uint32_t used_credits;
    };

    ACTION addsettings (name key, uint32_t value);

    [[eosio::on_notify("eosio.token::transfer")]] 
    void buycredits(name from, name to, asset quantity, string memo);
    ACTION protocolfees(name org, name feature);
    ACTION ramcredits(name org, name contract, uint64_t bytes, string memo);
    ACTION notify (name org, uint32_t total_credits, uint32_t used_credits);
    

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

    bool check_internal_auth (name action) {
      auth_table _auth(name(AUTHORITY_CONTRACT), name(AUTHORITY_CONTRACT).value);

      // Find the authority entry
      auto secondary_key = (uint128_t)(name(get_self()).value << 64 | action.value);
      auto secondary_index = _auth.get_index<"bycontract"_n>();
      auto itr = secondary_index.find(secondary_key);

      if (itr == secondary_index.end() || itr->contract!=name(get_self()) || itr->action!=action) {
          return false; // No authority found for the specified contract and action
      }
      auto authorzied_accounts = itr->authorized_contracts;
      for(auto i = 0 ; i < authorzied_accounts.size(); i++ ) {
        if(has_auth(authorzied_accounts[i])) {
          return true;
        }
      }
      return false;
    }
    
    TABLE credits {
      name    org;
      uint32_t  total_credits;
      uint32_t used_credits;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("credits"), credits> credits_table;

    TABLE settings {
      name key;
      uint32_t value;
      auto primary_key() const { return key.value; }
    };
    typedef multi_index<name("settings"), settings> settings_table;

    uint32_t bytes_to_credits (uint64_t bytes) {
      uint32_t bytes_per_credit = getvalue(name(BYTES_PER_CREDIT));
      uint32_t credits = bytes/bytes_per_credit + (bytes%bytes_per_credit != 0);
      return credits;
    }

    uint32_t token_amount_to_credits (uint64_t amount) {
      uint64_t bytes_bought = getvalue(name(RAM_IN_BYTES_PER_SYS_TOKEN)) * amount/(getvalue(name(BYTES_PER_CREDIT))*10000);
      //uint32_t credits = bytes_to_credits (bytes_bought);
      return bytes_bought;
    }

    uint32_t getvalue (name key) {
      settings_table _settings(get_self(), get_self().value);
      auto itr = _settings.find(key.value);
      check(itr != _settings.end(), "Missing <key> in settings table");
      return itr->value;
    }

  void deduct_credit (name org, uint32_t credits) {
    credits_table _credits(get_self(), get_self().value);
    auto credits_itr = _credits.find(org.value);
    
    check(credits_itr != _credits.end(), "<org> never registered for credits");
    check(credits_itr->total_credits - credits_itr->used_credits - credits >= 0, "Credits exhausted for <org>");
    
    _credits.modify(credits_itr, get_self(), [&](auto& row) {
      row.used_credits = row.used_credits + credits;
    });

    if(credits_itr->total_credits / (credits_itr->used_credits + credits) < .9) {
      action {
        permission_level{get_self(), name("active")},
        get_self(),
        name("notify"),
        notify_args {
          .org = org,
          .total_credits = credits_itr->total_credits,
          .used_credits = credits_itr->used_credits + credits}
      }.send();
    } 
  }

};
