#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT "simplebadgez"
#define GOTCHABADGE_CONTRACT "gotchabadgex"
#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define HLL_EMITTER_CONTRACT "hllemitterxx"
#define CUMULATIVE_CONTRACT "cumulativezz"
#define ORG_CHECKS_CONTRACT_NAME "interface111"
#define AND_EMITTER_CONTRACT "andemitterxx"
#define STATISTICS_CONTRACT "statisticszz"
#define GIVERREP_CONTRACT "giverrepxxxx"
#define REPUTATION_MANAGER_VALIDATION_CONTRACT "repvalxxxxxx"


CONTRACT repmanager : public contract {
  public:
    using contract::contract;
  
    struct asset_contract_name {
      name issuing_contract;
      name asset_name;

      bool operator<(const asset_contract_name& ad) const {
        if (this->issuing_contract == ad.issuing_contract) {
          return (this->asset_name.to_string() < ad.asset_name.to_string());
        } else {
          return (this->issuing_contract.to_string() < ad.issuing_contract.to_string());
        }
      }
    };


  ACTION mrrep (name org,
    name authorized,
    name rep_badge,
    string rep_badge_offchain_lookup_data,
    string rep_badge_onchain_lookup_data,
    name mutual_recognition_badge, 
    string mutual_recognition_offchain_lookup_data, 
    string mutual_recognition_onchain_lookup_data,
    uint64_t mutual_recognition_badge_rep_multiplier,
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    name sender_uniqueness_badge,
    string sender_uniqueness_badge_offchain_lookup_data, 
    string sender_uniqueness_badge_onchain_lookup_data,
    uint64_t sender_uniqueness_badge_rep_multiplier,
    name giver_rep_badge,
    string giver_rep_badge_offchain_lookup_data, 
    string giver_rep_badge_onchain_lookup_data,
    uint64_t giver_rep_badge_rep_multiplier_per_hundred_badges);

  private:
    TABLE checks {
      name org;
      name checks_contract;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("checks"), checks> checks_table;

    void notify_checks_contract(name org) {
      checks_table _checks( name(ORG_CHECKS_CONTRACT_NAME), get_self().value );
      auto itr = _checks.find(org.value);
      if(itr != _checks.end()) {
        require_recipient(itr->checks_contract);
      }
    }
    
    struct addfeature_args {
      name org;
      name badge_name;
      name notify_account;
      string memo;
    };
    
    struct creategotcha_args {
      name org;
      name badge;
      time_point_sec starttime;
      uint64_t cycle_length;
      uint8_t supply_per_cycle;
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

    struct hllemitter_newemission_args {
      name org;
      name badge;
      name sender_uniqueness_badge;
    };
    
    struct createsimple_args {
      name org;
      name badge;
      vector<name> parent_badges;
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

    struct andemitter_newemission_args { 
      name org;
      name emission_name;
      std::map<name, uint64_t> emitter_criteria;
      std::map<asset_contract_name, uint64_t> emit_assets;
      bool cyclic;
    };

    struct andemitter_activate_args {
      name org;
      name emission_name;
    };

    struct giverrep_newemission_args {
      name org;
      name badge;
      name source_account_rep_badge; 
      name emit_badge;
    };
};
