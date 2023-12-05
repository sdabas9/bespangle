#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define BILLING_CONTRACT "billingxxxxx"
#define NOTIFICATION_CONTRACT "notification"
#define AUTHORITY_CONTRACT "authorityxxx"


#define ADD_FEATURE_NOTIFICATION NOTIFICATION_CONTRACT"::addfeature"

CONTRACT metadata : public contract {
  public:
    using contract::contract;
 
  ACTION recognize (name trusted_badge_contract);

 /* ACTION isrecognized (name org, name contract); */

  ACTION initbadge (name org, 
    name badge,
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo);

  ACTION extaddfeatur (name org, 
    name badge,
    name notify_account,
    string memo);

  ACTION addfeature (name org, 
    name badge,
    name notify_account,
    string memo);
  
  [[eosio::on_notify(ADD_FEATURE_NOTIFICATION)]] void naddfeatur (name org, 
    name badge,
    name notify_account,
    string memo);

  ACTION addnotify(
    name org,
    name badge,
    name notify_accounts,
    string memo, 
    string offchain_lookup_data,
    string onchain_lookup_data,
    uint64_t rarity_counts); 

  ACTION delfeature (name org, 
    name badge,
    name notify_account,
    string memo);

  ACTION delnotify(
    name org,
    name badge,
    name notify_accounts,
    string memo,
    string offchain_lookup_data,
    string onchain_lookup_data,
    uint64_t rarity_counts); 
  
  ACTION achievement (name org,
    name badge,
    name account,
    name from,
    uint64_t count,
    string memo);

  ACTION notifyachiev (name org, 
    name badge,
    name account, 
    name from,
    uint64_t count,
    string memo,  
    vector<name> notify_accounts);

  ACTION mergeinfo (name org, 
    name badge, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo);

  private:

    struct ramcredits_arg {
      name org;
      name contract;
      uint64_t bytes;
      string memo;
    };

    struct syscredits_arg {
      name org;
    };

    struct notifyachievement_args {
      name org;
      name badge;
      name account;
      name from;
      uint64_t count;
      string memo;
      vector<name> notify_accounts;
    };

    struct downstream_notify_args {
      name org;
      name badge;
      name notify_account;
      string memo;
      string offchain_lookup_data;
      string onchain_lookup_data;
      uint64_t rarity_counts;
    };

    struct checkallow_args {
      name org;
      name account;
    };

    struct local_addfeature_args {
      name org; 
      name badge;
      name notify_account;
      string memo;
    };

    TABLE authorized {
      name trusted_contract;
      auto primary_key() const {return trusted_contract.value; }
    };
    typedef multi_index<name("authorized"), authorized> authorized_contracts_table;

    TABLE badge {
      name badge;
      vector<name> notify_accounts;
      string offchain_lookup_data;
      string onchain_lookup_data;
      uint64_t rarity_counts;
      auto primary_key() const {return badge.value; }
    };
    typedef multi_index<name("badge"), badge> badge_table;

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
    
    bool check_authorization () {

      authorized_contracts_table _authorized_contracts( _self, _self.value );
      for(auto itr = _authorized_contracts.begin(); itr != _authorized_contracts.end(); ++itr ) {
        
        if (has_auth(itr->trusted_contract)) {
          return true;
        }
      }
      check(false, "metadata; check_authorization - action does not have authorization of any trusted contract");
    }

    void init (name org, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
      badge_table _badge( _self, org.value );
      auto badge_iterator = _badge.find (badge.value);
      check(badge_iterator == _badge.end() ,
       "<badge> already exists for <org> ");
      /* ipfs_hashes.push_back ( ipfs_hash {
        .key = "img",
        .value = ipfs_image
      }); */
      _badge.emplace(get_self(), [&](auto& row){
        row.badge = badge;
        row.offchain_lookup_data = offchain_lookup_data;
        row.onchain_lookup_data = onchain_lookup_data;
        row.rarity_counts = 0;
      });
     // deduct_credit (org, 291 + ipfs_image.length(), "native init : <org> <contract> <badge>");
    }

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

    void deduct_platform_fees (name org) {
      action {
        permission_level{get_self(), name("active")},
        name(BILLING_CONTRACT),
        name("sysredits"),
        syscredits_arg {
          .org = get_self()}
      }.send();
    }

    /*void check_account_prefs (name org, name account) {
      action {
        permission_level{get_self(), name("active")},
        name(ACCOUNT_PREFERENCES_CONTRACT),
        name("checkallow"),
        checkallow_args {
          .org = org,
          .account = account }
      }.send();
    }*/
};
