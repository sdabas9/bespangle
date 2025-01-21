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


    TABLE bounty {
      name bounty;
      name emission;
      name status;
      time_point_sec participation_start_time;
      time_point_sec participation_end_time;
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
