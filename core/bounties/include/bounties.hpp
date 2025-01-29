#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define AND_EMITTER_MANAGER_CONTRACT "sdfasdf"
#define MSIG_CONTRACT "sdfasfd"
#define ORG_CONTRACT "org"

CONTRACT bounties : public contract {
  public:
    using contract::contract;


    ACTION cappedbounty(name creator,
       name org,
       string description,
       vector<asset> emit_assets,
       extended_asset fungible_asset_per_winner,
       vector<name> reviewers, 
       vector<name> participants,
       time_point_sec participation_start_time,
       time_point_sec participation_end_time);
       
    
    ACTION beginbounty();
    ACTION pausebounty();
    ACTION endbounty();

    ACTION inprogress(name bounty, name participant);
    ACTION submit(name bounty, name participant);

  private:

    void on_transfer(name from, name to, asset amount, string memo );

    TABLE bounty {
      symbol bounty_symbol;
      symbol badge_symbol;
      symbol emission_symbol;
      name status;
      vector<name> reviewers;
      vector<name> defined_participants;
      asset total_assets_deposited;
      asset total_assets_distributed;
      asset per_winner_amount;
      string json_details;
      time_point_sec participation_start_time;
      time_point_sec participation_end_time;
      uint64_t max_entries_per_participants;
      uint64_t max_number_of_participants;
    };
    struct status_timestamp {
      name status;
      time_point_sec status_change_time;
    };
    TABLE participants {
      uint64_t participation_id;
      name participant;
      name status;
      time_point_sec status_change_time;
    };

        // Table to store the next badge code
    TABLE badgecode {
        name org;             // Organization
        string next_badge_code; // Next badge code

        uint64_t primary_key() const { return org.value; }
    };

    // Table to store the next badge code
    TABLE emissioncode {
        name org;             // Organization
        string next_emission_code; // Next bounty code

        uint64_t primary_key() const { return org.value; }
    };

    TABLE bountycode {
        name org;             // Organization
        string next_bounty_code; // Next badge code

        uint64_t primary_key() const { return org.value; }
    };

    typedef multi_index<"badgecode"_n, badgecode> badgecode_table;
    typedef multi_index<"emissioncode"_n, emissioncode> emissioncode_table;
    typedef multi_index<"bountycode"_n, bountycode> bountycode_table;


    struct newemissiona_args {
      name authorized; 
      symbol emission_symbol; 
      vector<asset> emitter_criteria; 
      vector<asset> emit_badges;
      extended_asset fungible_asset; 
      bool cyclic;
    };

    struct 

};
