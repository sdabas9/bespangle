#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include "org.hpp"
#include "andemitter.hpp"
#include "authority.hpp"

using namespace std;
using namespace eosio;

#define REQUESTS_CONTRACT "requestsyyyy"
#define SIMPLEBADGE_CONTRACT "simplebadgey"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define CUMULATIVE_CONTRACT "cumulativeyy"
#define STATISTICS_CONTRACT "statisticsyy"

#define STATUS_CHANGE_NOTIFICATION REQUESTS_CONTRACT"::statuschange"

CONTRACT bounties : public contract {
  public:
    using contract::contract;

    struct contract_asset {
        name contract;
        asset emit_asset;
    };
   /**
    * @brief Create a new capped bounty.
    *
    * @param creator                           Account creating the bounty.
    * @param badge_symbol                      The symbol for the badge (used in other actions).
    * @param offchain_lookup_data              Off-chain lookup info.
    * @param onchain_lookup_data               On-chain lookup info.
    * @param detailed_description_ipfs         IPFS hash or link for detailed description.
    * @param emit_assets                       Assets used for emission badges.
    * @param total_fungible_assets             Total fungible assets for the bounty.
    * @param capped_participation              Whether participation is capped.
    * @param max_submissions_per_participants      Maximum entries allowed per participant.
    * @param max_number_of_participants        Maximum number of participants.
    * @param max_fungible_assets_payout_per_winner   Max assets payout per winner.
    * @param closed_participants               Whether participants list is closed.
    * @param participants_list                 List of participants.
    * @param reviewers                         List of reviewers.
    * @param participation_start_time          Start time for participation.
    * @param participation_end_time            End time for participation.
    * @param bounty_settlement_time            Settlement time for the bounty.
    */
   ACTION cappedbounty( name             authorized,
                        name                       payer,
                        symbol                     badge_symbol,
                        bool                       new_badge,
                        string                     badge_offchain_lookup_data,
                        string                     badge_onchain_lookup_data,
                        bool                       lifetime_aggregate,
                        bool                       lifetime_stats,
                        string                     emission_offchain_lookup_data,
                        string                     emission_onchain_lookup_data,
                        vector<asset>              emit_assets,
                        vector<extended_asset>     total_fungible_assets,
                        bool                       capped_participation,
                        uint16_t                   max_submissions_per_participant,
                        uint64_t                   max_number_of_participants,
                        vector<extended_asset>     max_fungible_assets_payout_per_winner,
                        bool                       closed_participants,
                        vector<name>               participants_list,
                        vector<name>               reviewers,
                        time_point_sec             participation_start_time,
                        time_point_sec             participation_end_time,
                        time_point_sec             bounty_settlement_time );
       
   /**
    * @brief Add new reviewers to an existing bounty.
    *
    * This action looks up the bounty record by `emission_symbol` and adds each
    * reviewer from the input vector to the bounty’s reviewers vector if they are not already present.
    *
    * @param authorized      The account authorized to modify the bounty.
    * @param emission_symbol The symbol identifying the bounty record.
    * @param reviewers       A vector of reviewer names to add.
    */
   ACTION addreviewers(name authorized, symbol badge_symbol, vector<name> reviewers);


   /**
    * @brief Add new participants to an existing bounty.
    *
    * This action looks up the bounty record by `emission_symbol` and then adds each
    * participant from the input vector to the bounty’s participants map if they are not already present.
    * **This action only proceeds if `closed_participants` is true.**
    *
    * @param authorized      The account authorized to modify the bounty.
    * @param emission_symbol The symbol identifying the bounty record.
    * @param participants    A vector of participant names to add.
    */
   ACTION addparts(name authorized, symbol badge_symbol, vector<name> participants);
    

   
   [[eosio::on_notify("*::transfer")]]
   void ontransfer(name from, name to, asset amount, string memo);


   ACTION signup(name participant, symbol badge_symbol, string stream_reason);
   ACTION cancelsignup(name participant, symbol badge_symbol, string stream_reason);   
   ACTION submit(name participant, symbol badge_symbol, string stream_reason);

   ACTION closebounty(symbol badge_symbol);
   ACTION withdraw(name account, symbol badge_symbol);// returns refund

   ACTION cleanup(symbol badge_symbol);

   [[eosio::on_notify(STATUS_CHANGE_NOTIFICATION)]]
   void status(name requester, uint64_t request_id, name originating_contract, name originating_contract_key, name old_status, name new_status);


    
  private:

   // Struct definitions for inline action arguments

   // For calling nextemission in the andemitter contract.
   struct nextemission_args {
      name org;
   };

   // For calling initsimple in the simmanager contract.
    struct createsimple_args {
      name org;
      symbol badge_symbol;
      string offchain_lookup_data; 
      string onchain_lookup_data;
      string memo;
    };

   // For calling newemission in the aemanager contract.
   struct new_emission_args {
      name org;
      symbol emission_symbol;
      string offchain_lookup_data;
      string onchain_lookup_data;
      vector<asset> emitter_criteria;
      vector<contract_asset> emit_assets;
      bool cyclic;
   };

   struct activate_emission_args {
      name org;
      symbol emission_symbol;
   };

   struct deactivate_emission_args {
      name org;
      symbol emission_symbol;
   };

   struct addfeature_args {
      name org;
      symbol badge_symbol;
      name notify_account;
      string memo;
   };

   // Bounty table definition with new state_counts column.
   TABLE bounty {
      symbol                     emission_symbol;
      symbol                     badge_symbol;
      name                       status;
      vector<extended_asset>     total_fungible_assets;
      vector<extended_asset>     total_fungible_assets_deposited;
      bool                       capped_participation;
      uint16_t                   max_submissions_per_participant;
      uint64_t                   max_number_of_participants;
      uint64_t                   num_participants = 0;
      vector<extended_asset>     max_fungible_assets_payout_per_winner;
      bool                       closed_participants;
      // Participants map: when a participant signs up, we record an entry.
      // For closed participation, they must already be present.
      // The value is set to 1 on sign‐up.
      map<name, uint64_t>        participants;
      // Submissions map: records the number of submissions per participant.
      map<name, uint64_t>        submissions;
      vector<name>               reviewers;
      time_point_sec             participation_start_time;
      time_point_sec             participation_end_time;
      time_point_sec             bounty_settlement_time;
      map<name, uint64_t>        state_counts; // New column for state counts
      name                       payer;

      uint64_t primary_key() const { return badge_symbol.code().raw(); }
   };
   typedef multi_index<"bounty"_n, bounty> bounty_table;

   // TABLE: bountypool
   // ------------------------------
   // This table is scoped by the bounty's emission symbol code.
   TABLE bountypool {
      name account; // Depositing account (non-payer deposits)
      vector<extended_asset> total_fungible_assets_deposited;
      
      uint64_t primary_key() const { return account.value; }
   };
   // When instantiating bountypool_table, the scope is set to the bounty's symbol code.
   typedef multi_index<"bountypool"_n, bountypool> bountypool_table;

   // ------------------------------
   // TABLE: settings
   // ------------------------------
   // Stores various configuration settings.
   TABLE settings {
      name key;
      uint64_t value;
      
      uint64_t primary_key() const { return key.value; }
   };
   typedef multi_index<"settings"_n, settings> settings_table;

   void distribute(symbol badge_symbol, name account) {
      
      // Lookup the bounty record.
      bounty_table bounty_tbl(get_self(), get_self().value);
      auto b_itr = bounty_tbl.find(badge_symbol.code().raw());
      check(b_itr != bounty_tbl.end(), "Bounty record not found.");

      // Retrieve the approved count from state_counts (using key "approved"_n).
      auto appr_itr = b_itr->state_counts.find("approved"_n);
      check(appr_itr != b_itr->state_counts.end(), "Approved count not found in state_counts.");
      uint64_t approved = appr_itr->second;
      check(approved > 0, "Approved count must be greater than zero.");

      // Retrieve fee percentage from settings table.
      settings_table settings(get_self(), get_self().value);
      auto fee_itr = settings.find("fees"_n.value);
      uint64_t fee_bp = 0; // fee in basis points (e.g., 100 for 1%)
      if (fee_itr != settings.end()) {
         fee_bp = fee_itr->value;
      }

      // Vectors to accumulate net amounts and fee amounts for transfer.
      vector<extended_asset> net_assets;
      vector<extended_asset> fee_assets;

      // Lambda to process deposit records.
      // The boolean flag 'applyCap' indicates whether to enforce the cap from max_fungible_assets_payout_per_winner.
      auto process_deposits = [&](const vector<extended_asset>& deposits, bool applyCap) {
         for (auto const &dep : deposits) {
            // Verify the deposit asset is one of the bounty’s target assets.
            bool valid = false;
            for (auto const &target : b_itr->total_fungible_assets) {
               if (target.quantity.symbol == dep.quantity.symbol && target.contract == dep.contract) {
                  valid = true;
                  break;
               }
            }
            check(valid, "Deposit asset not recognized in bounty target assets.");
            // Compute the distribution amount.
            int64_t total_deposited = dep.quantity.amount;
            int64_t computed = total_deposited / approved;  // integer division
            int64_t final_amt = computed;
            if (applyCap) {
               // Look up the cap for this asset from max_fungible_assets_payout_per_winner.
               int64_t max_allowed = 0;
               bool found_cap = false;
               for (auto const &cap_ext : b_itr->max_fungible_assets_payout_per_winner) {
                  if (cap_ext.quantity.symbol == dep.quantity.symbol && cap_ext.contract == dep.contract) {
                     max_allowed = cap_ext.quantity.amount;
                     found_cap = true;
                     break;
                  }
               }
               if (found_cap && final_amt > max_allowed) {
                  final_amt = max_allowed;
               }
            }
            // Apply fees.
            int64_t fee_amt = (final_amt * fee_bp) / 10000;  // fee fraction = fee_bp/10000
            int64_t net_amt = final_amt - fee_amt;
            if (net_amt > 0) {
               asset net_asset = asset(net_amt, dep.quantity.symbol);
               extended_asset ext_net { net_asset, dep.contract };
               net_assets.push_back(ext_net);
            }
            if (fee_amt > 0) {
               asset fee_asset = asset(fee_amt, dep.quantity.symbol);
               extended_asset ext_fee { fee_asset, dep.contract };
               fee_assets.push_back(ext_fee);
            }
         }
      };

      process_deposits(b_itr->total_fungible_assets_deposited, true);
      bountypool_table pool_tbl(get_self(), badge_symbol.code().raw());
      auto pool_itr = pool_tbl.begin();
      while(pool_itr != pool_tbl.end()) {
         process_deposits(pool_itr->total_fungible_assets_deposited, false);
         pool_itr ++;
      }

      // For each asset computed, perform two transfers:
      // one to the target account (net amount) and one to the treasury account (fee amount).
      for (auto const &net_ext : net_assets) {
         action(
            permission_level{ get_self(), "active"_n },
            net_ext.contract,
            "transfer"_n,
            std::make_tuple(get_self(), account, net_ext.quantity, string("Distribution from bounty"))
         ).send();
      }
      for (auto const &fee_ext : fee_assets) {
         action(
            permission_level{ get_self(), "active"_n },
            fee_ext.contract,
            "transfer"_n,
            std::make_tuple(get_self(), "treasury"_n, fee_ext.quantity, string("Fee from distribution"))
         ).send();
      }
   }


   // ------------------------------
   // Remote Action Struct for consumesimple (from the "requests" contract)
   // ------------------------------
   struct consumesimple_args {
      name           originating_contract;
      name           originating_contract_key;
      name           requester;
      vector<name>   approvers;
      name           to;
      symbol         badge_symbol;
      uint64_t       amount;
      string         memo;
      string         request_memo;
      time_point_sec expiration_time;
   };


   void new_emission(symbol emission_symbol, 
   string offchain_lookup_data,
   string onchain_lookup_data,
   vector<asset> emitter_criteria, 
   vector<asset> emit_badges, 
   bool cyclic) {

   string action_name = "new_emission";
   string failure_identifier = "CONTRACT: aemanager, ACTION: " + action_name + ", MESSAGE: ";
   name org = get_org_from_internal_symbol(emission_symbol, failure_identifier);

   vector<contract_asset> emit_assets;

   for(asset a : emitter_criteria) {
      check(org == get_org_from_internal_symbol(emission_symbol, failure_identifier), "org not same for all assets in emitter_criteria");
   }
   for(asset a : emit_badges) {
      check(org == get_org_from_internal_symbol(emission_symbol, failure_identifier), "org not same for all assets in emit_badges");
   }

   for(auto i = 0 ; i < emit_badges.size(); i++) {
      emit_assets.push_back (contract_asset {
         .contract = name(SIMPLEBADGE_CONTRACT),
         .emit_asset = emit_badges[i]
      });
   }

   action{
      permission_level{get_self(), name("active")},
      name(ANDEMITTER_CONTRACT),
      name("newemission"),
      new_emission_args{
         .org = org,
         .emission_symbol = emission_symbol,
         .offchain_lookup_data = offchain_lookup_data, 
         .onchain_lookup_data = onchain_lookup_data, 
         .emitter_criteria = emitter_criteria,
         .emit_assets = emit_assets,
         .cyclic = cyclic
      }
   }.send();

   for (auto i = 0; i < emitter_criteria.size(); i++) {
      action{
         permission_level{get_self(), name("active")},
         name(ORCHESTRATOR_CONTRACT),
         name("addfeature"),
         addfeature_args{
         .org = org,
         .badge_symbol = emitter_criteria[i].symbol,
         .notify_account = name(ANDEMITTER_CONTRACT),
         .memo = ""
         }
      }.send();
   }

   action{
      permission_level{get_self(), name("active")},
      name(ANDEMITTER_CONTRACT),
      name("activate"),
      activate_emission_args{
         .org = org,
         .emission_symbol = emission_symbol
      }
   }.send();
   }

   void deactivate_emission(symbol emission_symbol) {
   string action_name = "deactivate";
   string failure_identifier = "CONTRACT: aemanager, ACTION: " + action_name + ", MESSAGE: ";
   name org = get_org_from_internal_symbol(emission_symbol, failure_identifier);

   action{
      permission_level{get_self(), name("active")},
      name(ANDEMITTER_CONTRACT),
      name("deactivate"),
      deactivate_emission_args{
         .org = org,
         .emission_symbol = emission_symbol
      }
   }.send();
   }

};
