#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <vector>

using namespace std;
using namespace eosio;
using std::vector;

#define MUTUAL_RECOGNITION_MANAGER_CONTRACT "mrmanageryyy"
#define ORG_CONTRACT "organizayyyy"

#define MUTUAL_RECOGNITION_MANAGER_CREATE_NOTIFICATION MUTUAL_RECOGNITION_MANAGER_CONTRACT"::init" 
#define MUTUAL_RECOGNITION_MANAGER_CHANGE_START_TIME_NOTIFICATION MUTUAL_RECOGNITION_MANAGER_CONTRACT"::starttime" 
#define MUTUAL_RECOGNITION_MANAGER_CHANGE_CYCLE_LENGTH_NOTIFICATION MUTUAL_RECOGNITION_MANAGER_CONTRACT"::cyclelength" 
#define MUTUAL_RECOGNITION_MANAGER_CHANGE_CYCLE_SUPPLY_NOTIFICATION MUTUAL_RECOGNITION_MANAGER_CONTRACT"::cyclesupply" 


CONTRACT mrval : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(MUTUAL_RECOGNITION_MANAGER_CREATE_NOTIFICATION)]] void init(
        name authorized,
        symbol badge_symbol,
        time_point_sec starttime,
        uint64_t cycle_length,
        uint8_t supply_per_cycle,
        string offchain_lookup_data,
        string onchain_lookup_data,
        bool lifetime_aggregate,
        bool lifetime_stats,
        string memo
    );
    
    [[eosio::on_notify(MUTUAL_RECOGNITION_MANAGER_CHANGE_START_TIME_NOTIFICATION)]] void starttime(
        name authorized,
        symbol badge_symbol,
        time_point_sec new_starttime
    );

    [[eosio::on_notify(MUTUAL_RECOGNITION_MANAGER_CHANGE_CYCLE_LENGTH_NOTIFICATION)]] void cyclelength(
        name authorized,
        symbol badge_symbol,
        uint64_t new_cycle_length
    );

    [[eosio::on_notify(MUTUAL_RECOGNITION_MANAGER_CHANGE_CYCLE_SUPPLY_NOTIFICATION)]] void cyclesupply(
        name authorized,
        symbol badge_symbol,
        uint8_t new_supply_per_cycle
    );
      
    ACTION addbadgeauth(name org, name action, name badge, name authorized_account);

    ACTION delbadgeauth(name org, name action, name badge, name authorized_account);

    ACTION addactionauth(name org, name action, name authorized_account);

    ACTION delactionauth(name org, name action, name authorized_account);


  private:
    TABLE actionauths {
      name  action;
      vector<name>  authorized_accounts;
      auto primary_key() const { return action.value; }
    };
    typedef multi_index<name("actionauths"), actionauths> actionauths_table;

    TABLE badgeauths {
      uint64_t id;
      name action;
      name badge;
      vector<name> authorized_accounts;

      uint64_t primary_key() const { return id; }
      uint128_t secondary_key() const { return combine_names(action, badge); }

      static uint128_t combine_names(const name& a, const name& b) {
          return (uint128_t{a.value} << 64) | b.value;
      }
    };

    typedef eosio::multi_index<"badgeauths"_n, badgeauths,
        indexed_by<"actionemiss"_n, const_mem_fun<badgeauths, uint128_t, &badgeauths::secondary_key>>
    > badgeauths_table;

    bool has_action_authority (name org, name action, name account) {
      actionauths_table _actionauths_table (get_self(), org.value);
      auto itr = _actionauths_table.find(action.value);
      if(itr == _actionauths_table.end()) {
        return false;
      }
      auto authorized_accounts = itr->authorized_accounts;
      for(auto i = 0; i < authorized_accounts.size(); i++) {
        if(authorized_accounts[i] == account) {
          return true;
        }
      }
      return false;
    }

    bool has_badge_authority (name org, name action, name badge, name account) {
      badgeauths_table badgeauths(get_self(), org.value);
      auto secondary_index = badgeauths.get_index<"actionemiss"_n>();
      uint128_t secondary_key = badgeauths::combine_names(action, badge);
      auto itr = secondary_index.find(secondary_key);
      if(itr == secondary_index.end() || itr->action != action || itr->badge != badge) {
        return false;
      }
      auto authorized_accounts = itr->authorized_accounts;
      for(auto i = 0; i < authorized_accounts.size(); i++) {
        if(authorized_accounts[i] == account) {
          return true;
        }
      }
      return false;
    }

    TABLE orgcode {
      name org;         // Organization identifier, used as primary key
      name org_code;    // Converted org_code, ensuring uniqueness and specific format

      // Specify the primary key
      auto primary_key() const { return org.value; }

      // Specify a secondary index for org_code to ensure its uniqueness
      uint64_t by_org_code() const { return org_code.value; }
    };

    // Declare the table
    typedef eosio::multi_index<"orgcodes"_n, orgcode,
      eosio::indexed_by<"orgcodeidx"_n, eosio::const_mem_fun<orgcode, uint64_t, &orgcode::by_org_code>>
    > orgcode_index;


    name get_name_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
        string _symbol_str = _symbol.code().to_string(); // Convert symbol to string
        check(_symbol_str.size() == 7, failure_identifier + "Symbol must have at least 7 characters.");

        // Extract the first 4 characters as org_code
        string _str = _symbol_str.substr(4, 7);

        for (auto & c: _str) {
            c = tolower(c);
        }
        return name(_str);
    }

    name get_org_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
      string _symbol_str = _symbol.code().to_string(); // Convert symbol to string
      check(_symbol_str.size() >= 4, failure_identifier + "symbol must have at least 4 characters.");

      // Extract the first 4 characters as org_code
      string org_code_str = _symbol_str.substr(0, 4);

      for (auto & c: org_code_str) {
          c = tolower(c);
      }
      name org_code = name(org_code_str);

      // Set up the orgcode table and find the org_code
      orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
      auto org_code_itr = orgcodes.get_index<"orgcodeidx"_n>().find(org_code.value);

      check(org_code_itr != orgcodes.get_index<"orgcodeidx"_n>().end(), failure_identifier + "Organization code not found.");
      check(org_code_itr->org_code == org_code, failure_identifier + "Organization code not found.");
      // Assuming the org is stored in the same row as the org_code
      return org_code_itr->org; // Return the found organization identifier
    }
};
