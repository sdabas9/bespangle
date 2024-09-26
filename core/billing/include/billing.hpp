#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

CONTRACT billing : public contract {
  public:
    using contract::contract;

  [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts);

  [[eosio::on_notify("eosio.token::transfer")]]
    void receive(name from, name to, asset quantity, string memo);

    ACTION membercost(asset cost);

    ACTION addpackage(asset cost, uint64_t number_of_issuances, uint64_t expiry_duration);

    ACTION activatep(uint64_t seq);

    ACTION deactivatep(uint64_t seq);


  private:

    // Define the packs table
    TABLE packs {
      uint64_t seq_id;
      asset cost;
      uint64_t number_of_issuances;
      uint64_t expiry_duration;  // Expiry duration in seconds

      uint64_t primary_key() const { return seq_id; }
    };

    typedef eosio::multi_index<"packs"_n, packs> packs_table;

    // Define the orgpacks table
    TABLE orgpacks {
      uint64_t seq_id;
      name status;               // expired, active, available
      uint64_t allowed;
      uint64_t used;
      time_point expiry_time;    // Time when the pack expires

      uint64_t primary_key() const { return seq_id; }

      // Secondary index based on expiry_time
      uint64_t get_expiry_time() const { return expiry_time.elapsed.count(); }

      // Secondary index based on status
      uint64_t get_status() const { return status.value; }
    };

    typedef eosio::multi_index<"orgpacks"_n, orgpacks,
        indexed_by<"byexpiry"_n, const_mem_fun<orgpacks, uint64_t, &orgpacks::get_expiry_time>>,
        indexed_by<"bystatus"_n, const_mem_fun<orgpacks, uint64_t, &orgpacks::get_status>>
    > orgpacks_table;





    TABLE orgcurrpack {
      uint64_t allowed;
      uint64_t used;
      time_point_sec expiry_date;
    }
    TABLE issuance {
      name  org;
      vector<uint16_t> unique_issuance_hll;
      vector<uint16_t> unique_member_hll;
      uint64_t unique_issuance_allowed;
      uint64_t unique_issuance_count;
      uint64_t unique_member_count;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("issuance"), issuance> issuance_table;

    TABLE membercost {
      asset cost;
    };
    typedef multi_index<name("settings"), settings> settings_table;
    // key membercost
    // key paygrace



    TABLE freesub {
      name org;
      uint64_t free_members;
      time_point_sec expiry_date;
    };

    TABLE freeissuance {
      name org;
      uint64_t free_issuances_given;
    };

    TABLE subscription {
      name org;
      time_point_sec due_date;
      asset due_amount;
      time_point_sec cycle_start_date;
      time_point_sec cycle_end_date;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("subscription"), subscription> subscription_table;

};
