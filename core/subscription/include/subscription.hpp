#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>
#include <eosio/time.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"

CONTRACT subscription : public contract {
  public:
    using contract::contract;

  /* action to buy a package. memo is of format <ORG>:PACK:<package_name> OR <ORG>:ADMIN. 
  if of format <ORG>:PACK:<package_name>. Has to be active package in packages table. inserts record in newpackage table
  Returns additional balance.
  */ 
  [[eosio::on_notify("eosio.token::transfer")]]
  void buyservice (name from, name to, asset amount, string memo);

  /*
  1) checks if there is an available package.
  2) updates total_used in currpackages.
  3) if curr package is expired (either by expiry_time or total used is same as total bought), then move record from currpackges table to usedpackages.
    if newpackge is available then insert record into currpackage table using info from packges table. if newpackge is not availble then throw error.
  2) checks if member payment is not due.
  3) updates member count in members table.
  */
  [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]]
  void notifyachiev (
      name org,
      asset badge_asset,
      name from,
      name to,
      string memo,
      vector<name> notify_accounts
  ); 


  // Inserts record in packages table with active as true. 
  ACTION newpack(name package,
      uint64_t unique_issuance_size,
      uint64_t expiry_duration,
      uint8_t active_seasons,
      asset ram_cost_without_volatility); 

  // turns active bool to false
  ACTION disablepack(name package);

  // tune active bool to true.
  ACTION enablepack(name package);

  // inserts record in mcost table
  ACTION membercost(asset first_member_cost);

  // inserts record in mduetime table
  ACTION graceperiod(uint64_t number_of_grace_seconds_for_admin_payment);



  // generates total due amount for an org in billed_amount_calculated_value in members table.
  // it is caluclated by getting number of members from member_count in members tables multiplied by first_member_usd_cost in mcost table . 
   //action can only execute within billing_calc_window seconds after start of month. 
  // can be only executed once per month. last execution time is stored in billed_amount_calculation_time in members table.
  ACTION genbill(name org);



  private:

// Table definition for packages
TABLE packages {
   name package;
   uint64_t unique_issuance_size;
   uint64_t expiry_duration;
   uint8_t active_seasons;
   asset ram_cost_without_volatility;
   bool active;

   uint64_t primary_key() const { return package.value; }
};
typedef eosio::multi_index<"packages"_n, packages> packages_table;

// Table definition for volatility
TABLE volatility {
   uint64_t seq_id;
   uint8_t value;
   time_point_sec last_updated;

   uint64_t primary_key() const { return seq_id; }
};
typedef eosio::multi_index<"volatility"_n, volatility> volatility_table;

// Singleton table definition for raminusd
TABLE raminusd {
   asset per_kb_usd_price;

   uint64_t primary_key() const { return 0; }
};
typedef eosio::singleton<"raminusd"_n, raminusd> raminusd_singleton;


// Table definition for newpackage
TABLE newpackage {
   name package_name;

   uint64_t primary_key() const { return package_name.value; }
};
typedef eosio::multi_index<"newpackage"_n, newpackage> newpackage_table;

// Singleton table definition for sequences
TABLE sequences {
   uint64_t last_package_seq_id;

   // Singleton pattern does not require primary key, but EOSIO expects one.
   uint64_t primary_key() const { return 0; }
};
typedef eosio::singleton<"sequences"_n, sequences> sequences_singleton;

// Table definition for currpackage
TABLE currpackage {
   uint64_t seq_id;
   uint64_t total_bought;
   uint64_t total_used;
   time_point_sec expiry_time;

   uint64_t primary_key() const { return seq_id; }
};
typedef eosio::multi_index<"currpackage"_n, currpackage> currpackage_table;

// Table definition for usedpackages
TABLE usedpackages {
   uint64_t seq_id;
   uint64_t total_bought;
   uint64_t total_used;
   time_point_sec expiry_time;

   uint64_t primary_key() const { return seq_id; }
};
typedef eosio::multi_index<"usedpackages"_n, usedpackages> usedpackages_table;


// Singleton table definition for admincost
TABLE admincost {
   asset first_member_usd_cost;

   uint64_t primary_key() const { return 0; }
};
typedef eosio::singleton<"admincost"_n, admincost> admincost_singleton;

// Singleton table definition for mduetime
TABLE mduetime {
   uint64_t due_grace_seconds;

   uint64_t primary_key() const { return 0; }
};
typedef eosio::singleton<"mduetime"_n, mduetime> mduetime_singleton;

// Singleton table definition for adminbill
TABLE adminbill {
   uint64_t billing_calc_window; // Should be less than due_grace_seconds.

   uint64_t primary_key() const { return 0; }
};
typedef eosio::singleton<"adminbill"_n, adminbill> adminbill_singleton;

// Table definition for members
TABLE members {
   name org;
   vector<uint8_t> member_count_hll;
   uint64_t member_count;
   time_point_sec first_subscription_time;
   time_point_sec last_billed_amount_calculation_time;
   asset last_billed_amount_calculated_value;
   time_point_sec last_billed_amount_paid_time;
   asset last_billed_amount_paid_value;
   asset current_balance;

   uint64_t primary_key() const { return org.value; }
};
typedef eosio::multi_index<"members"_n, members> members_table;





};
