#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "orchestrator"
#define AUTHORITY_CONTRACT "authorityzzz"
#define BILLING_CONTRACT "billingxxxxx"


CONTRACT seriesbadge : public contract {
  public:
    using contract::contract;

    ACTION createseries(name org, name series);
    ACTION createnext(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo);
    ACTION issuelatest(name org, name series, name to, uint64_t count, string memo);
    ACTION issueany(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo);

  private:
    TABLE metadata {
      name series;
      uint64_t seq_id;
      auto primary_key() const {return series.value; }
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

    TABLE badge {
      uint64_t id;
      name series;
      uint64_t seq_id;
      name badge;
      auto primary_key() const {return id; }

      uint128_t series_seq_id_key() const {
        return ((uint128_t) seq_id) << 64 | series.value;
      }
      uint128_t series_badge_key() const {
        return ((uint128_t) series.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("badge"), badge,
    indexed_by<name("seriesseqid"), const_mem_fun<badge, uint128_t, &badge::series_seq_id_key>>,
    indexed_by<name("seriesbadge"), const_mem_fun<badge, uint128_t, &badge::series_badge_key>>
    > badge_table;

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

    struct createseries_args {
      name org;
      name series;
    };

    struct createnext_args {
      name org;
      name series;
      name badge;
      string offchain_lookup_data;
      string onchain_lookup_data;
      string memo;
    };

    struct issuelatest_args {
      name org;
      name series;
      name to;
      uint64_t count;
      string memo;
    };

    struct issueany_args {
      name org;
      name series;
      uint64_t seq_id;
      name to;
      uint64_t count;
      string memo;
    };

    struct achievement_args {
      name org;
      name badge_name;
      name account;
      name from;
      uint64_t count;
      string memo;
    };

    struct initbadge_args {
      name org;
      name badge_name;
      string offchain_lookup_data; 
      string onchain_lookup_data; 
      string memo;
    }; 

};
