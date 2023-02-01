#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT_NAME "simplebadge2"
#define CLAIMASSET_CONTRACT_NAME "claimasset11"
#define NOTIFICATION_CONTRACT_NAME "notification"
#define ORCHESTRATOR_CONTRACT_NAME "metadata2222"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::notifyachiev"


CONTRACT rollup : public contract {
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

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev (
      name org, 
      name badge_contract, 
      name badge_name,
      name account, 
      name from,
      uint8_t count,
      string memo,
      uint64_t badge_id,  
      vector<name> notify_accounts);

    ACTION createrollup (name org,
      name emission_name,
      std::map<asset_contract_name, uint16_t> activator_criteria,
      std::map<asset_contract_name, uint16_t> emitter_criteria,
      std::map<asset_contract_name, uint8_t> emit_assets,
      bool cyclic_from_activator,
      bool cyclic_from_emitter);

    ACTION activate (name org, name emission_name);

    ACTION deactivate (name org, name emission_name);

    ACTION givesimple (name org, name to, name badge, uint8_t amount, string memo);

    ACTION addclaimer (name org, name account, name assetname, uint64_t account_cap);

  private:

    enum short_activator_status: uint8_t {
      ACTIVATOR_START= 0,
      ACTIVATOR_IN_PROGESS= 1,
      EMITTER_START = 2,
      EMITTER_IN_PROGRESS = 3,
      EMITTED = 4
    };

    TABLE emissions {
      name emission_name;
      std::map<asset_contract_name, uint16_t> activator_criteria;
      std::map<asset_contract_name, uint16_t> emitter_criteria;
      std::map<asset_contract_name, uint8_t> emit_assets;
      name status; // INIT, ACTIVATE, DEACTIVATE
      bool cyclic_from_activator;
      bool cyclic_from_emitter;
      auto primary_key() const { return emission_name.value; }
    };
    typedef multi_index<name("emissions"), emissions> emissions_table;

    TABLE emitters {
      uint64_t badge_id;
      vector<name> active_emissions;
      auto primary_key() const { return badge_id; }
    };
    typedef multi_index<name("emitters"), emitters> emitters_table;

    TABLE activators {
      uint64_t badge_id;
      vector<name> active_emissions;
      auto primary_key() const { return badge_id; }
    };
    typedef multi_index<name("activators"), activators> activators_table;

    TABLE accounts {
      uint64_t id;
      name account;
      name emission_name;
      uint8_t emission_status;
      std::map<asset_contract_name, uint16_t> expanded_activator_status;
      std::map<asset_contract_name, uint16_t> expanded_emitter_status;
      auto primary_key() const { return id; }
      uint128_t account_emission_name_key() const {
        return ((uint128_t) account.value) << 64 | emission_name.value;
      }
      uint64_t emission_name_key() const {
        return emission_name.value;
      } // TODO are these 2 keys needed?
    };
    typedef multi_index<name("accounts"), accounts,
    indexed_by<name("accntemssn"), const_mem_fun<accounts, uint128_t, &accounts::account_emission_name_key>>,
    indexed_by<name("emissionkey"), const_mem_fun<accounts, uint64_t, &accounts::emission_name_key>>    
    > accounts_table;

    TABLE badge {
      uint64_t badge_id;
      name badge_contract;
      name badge_name;
      vector<name> notify_accounts;
      string offchain_lookup_data;
      string onchain_lookup_data;
      uint32_t rarity_counts;
      auto primary_key() const {return badge_id; }
      uint128_t contract_badge_key() const {
        return ((uint128_t) badge_contract.value) << 64 | badge_name.value;
      }
    };
    typedef multi_index<name("badge"), badge,
    indexed_by<name("contractbadge"), const_mem_fun<badge, uint128_t, &badge::contract_badge_key>>    
    > badge_table;

    struct issue_args {
      name org;
      name to;
      name badge; 
      uint8_t amount;
      string memo;
    };

    struct addclaimer_args {
      name org;
      name account;
      name assetname; 
      uint64_t account_cap;
    };

    void invoke_action ( name org, std::map<asset_contract_name, uint8_t> emit_assets ,name to) {

      for (const auto& [key, value] : emit_assets) {
        if(key.issuing_contract == name(SIMPLEBADGE_CONTRACT_NAME)) {
          action {
            permission_level{get_self(), name("active")},
            name(get_self()),
            name("givesimple"),
            issue_args {
              .org = org,
              .to = to,
              .badge = key.asset_name,
              .amount = value,
              .memo = "issued from rollup consumer"}
          }.send(); 
        } else if (key.issuing_contract == name(CLAIMASSET_CONTRACT_NAME)) {
          action {
            permission_level{get_self(), name("active")},
            name(get_self()),
            name("addclaimer"),
            addclaimer_args {
              .org = org,
              .account = to,
              .assetname = key.asset_name,
              .account_cap = 1 }
          }.send(); 
        }

      }

    }

    uint64_t get_badge_id (name org, name issuing_contract, name assetname) {
      badge_table _badge(name(ORCHESTRATOR_CONTRACT_NAME), org.value);
      auto contract_badge_index = _badge.get_index<name("contractbadge")>();
      uint128_t contract_badge_key = ((uint128_t) issuing_contract.value) << 64 | assetname.value;
      auto contract_badge_iterator = contract_badge_index.require_find (contract_badge_key, "Could not find Contract, badge ");
      return contract_badge_iterator->badge_id;
    }

    bool matched_all_conditions (std::map<asset_contract_name, uint16_t> rollup_criteria, std::map<asset_contract_name, uint16_t> rollup_status) {
      for (const auto& [key, value] : rollup_criteria) {
        if (rollup_status[key] < rollup_criteria[key]) {
          return false;
        }
      }
      return true;
    }

    void activators (name org, name emission_name, name account, name issuing_contract, name asset_name, uint8_t count) {

      struct asset_contract_name received_asset = {.issuing_contract = issuing_contract,
      .asset_name = asset_name};

      accounts_table _accounts(_self, org.value);
      auto account_emission_index = _accounts.get_index<name("accntemssn")>();
      uint128_t account_emission_key = ((uint128_t) account.value) << 64 | emission_name.value;
      auto account_emission_iterator = account_emission_index.find (account_emission_key);

      emissions_table _emissions(_self, org.value);
      auto emissions_itr = _emissions.require_find(emission_name.value, "Somethings wrong");
      

      bool insert_row_in_accounts = account_emission_iterator == account_emission_index.end() || 
      account_emission_iterator->account != account || 
      account_emission_iterator->emission_name != emission_name;

      bool modify_row_in_accounts = account_emission_iterator != account_emission_index.end() && 
      account_emission_iterator->account == account && 
      account_emission_iterator->emission_name == emission_name && 
      (account_emission_iterator->emission_status == ACTIVATOR_IN_PROGESS || account_emission_iterator->emission_status == ACTIVATOR_START);



      if(insert_row_in_accounts) {

        std::map<asset_contract_name, uint16_t> expanded_activator_status;
        expanded_activator_status[received_asset] = count;
        bool start_emitter = matched_all_conditions(emissions_itr->activator_criteria, expanded_activator_status);

        _accounts.emplace(get_self(), [&](auto& row){
          if(start_emitter) {
            row.emission_status = EMITTER_START;
            expanded_activator_status.clear();
          } else {
            row.emission_status = ACTIVATOR_IN_PROGESS;
          }
          row.id = _accounts.available_primary_key();
          row.account = account;
          row.emission_name = emission_name;
          row.expanded_activator_status = expanded_activator_status;
        });
      } else if (modify_row_in_accounts) {
        std::map<asset_contract_name, uint16_t> expanded_activator_status = account_emission_iterator->expanded_activator_status;
        if ( expanded_activator_status.contains(received_asset) ) {
          expanded_activator_status[received_asset] = expanded_activator_status[received_asset] + count;
        } else {
          expanded_activator_status[received_asset] = count;
        }
        bool start_emitter = matched_all_conditions(emissions_itr->activator_criteria, expanded_activator_status);
        account_emission_index.modify(account_emission_iterator, get_self(), [&](auto& row){
          if(start_emitter) {
            row.emission_status = EMITTER_START;
            expanded_activator_status.clear();
          } else {
            row.emission_status = ACTIVATOR_IN_PROGESS;
          }
          row.expanded_activator_status = expanded_activator_status;
        });
      }
    }

    void emitters (name org, name emission_name, name account, name issuing_contract, name asset_name, uint8_t count) {
      struct asset_contract_name received_asset = {.issuing_contract = issuing_contract,
      .asset_name = asset_name};

      accounts_table _accounts(_self, org.value);
      auto account_emission_index = _accounts.get_index<name("accntemssn")>();
      uint128_t account_emission_key = ((uint128_t) account.value) << 64 | emission_name.value;
      auto account_emission_iterator = account_emission_index.find (account_emission_key);

      emissions_table _emissions(_self, org.value);
      auto emissions_itr = _emissions.require_find(emission_name.value, "Somethings wrong");


      bool insert_activated_row_in_accounts = (account_emission_iterator == account_emission_index.end() || 
      account_emission_iterator->account != account || 
      account_emission_iterator->emission_name != emission_name) && 
      emissions_itr->activator_criteria.size() == 0;

      bool modify_row_in_accounts = account_emission_iterator != account_emission_index.end() && 
      account_emission_iterator->account == account && 
      account_emission_iterator->emission_name == emission_name && 
      (account_emission_iterator->emission_status == EMITTER_START ||
      account_emission_iterator->emission_status == EMITTER_IN_PROGRESS);
      
      if(insert_activated_row_in_accounts) {
        std::map<asset_contract_name, uint16_t> expanded_emitter_status;
        expanded_emitter_status[received_asset] = count;
        bool emit = matched_all_conditions(emissions_itr->emitter_criteria, expanded_emitter_status);

        _accounts.emplace(get_self(), [&](auto& row){
          if(emit) {
            if(emissions_itr->cyclic_from_activator) {
              row.emission_status = ACTIVATOR_START;
            } else if (emissions_itr->cyclic_from_emitter) {
              row.emission_status = EMITTER_START;
            } else {
              row.emission_status = EMITTED;
            }
            expanded_emitter_status.clear();
            invoke_action (org, emissions_itr->emit_assets, account);
          } else {
            row.emission_status = EMITTER_IN_PROGRESS;
          }
          row.id = _accounts.available_primary_key();
          row.emission_name = emission_name;
          row.account = account;
          row.expanded_emitter_status = expanded_emitter_status;
        });
      } else if (modify_row_in_accounts){
        std::map<asset_contract_name, uint16_t> expanded_emitter_status = account_emission_iterator->expanded_emitter_status;
        if ( expanded_emitter_status.contains(received_asset) ) {
          expanded_emitter_status[received_asset] = expanded_emitter_status[received_asset] + count;
        } else {
          expanded_emitter_status[received_asset] = count;
        }
        bool emit = matched_all_conditions(emissions_itr->emitter_criteria, expanded_emitter_status);

        account_emission_index.modify(account_emission_iterator, get_self(), [&](auto& row){
        if(emit) {
          if(emissions_itr->cyclic_from_activator) {
            row.emission_status = ACTIVATOR_START;
          } else if (emissions_itr->cyclic_from_emitter) {
            row.emission_status = EMITTER_START;
          } else {
            row.emission_status = EMITTED;
          }
          expanded_emitter_status.clear();
          invoke_action (org, emissions_itr->emit_assets, account);
        } else {
          row.emission_status = EMITTER_IN_PROGRESS;
        }
          row.expanded_emitter_status = expanded_emitter_status;
        });
      }
    }

};
