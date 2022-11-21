#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

#define SIMPLEBADGE_CONTRACT "simplebadge2"
#define GOTCHABADGE_CONTRACT "gotchabadge1"
#define ORCHESTRATOR_CONTRACT "metadata2222"
#define AABADGE_CONTRACT "aabadge11111"
#define CUMULATIVE_CONTRACT "cumulative11"
#define ROUNDS_CONTRACT "rounds111111"
#define ACCOUNT_PREFERENCES_CONTRACT "userprefs111"
#define SERIESBADGE_CONTRACT "seriesbadge1"

using namespace std;
using namespace eosio;

CONTRACT org : public contract {
  public:
    using contract::contract;

		/*
		* Initializes system for the org.
		*
		* This action registers the params need for the system to work. Details about params below.
		*
		* @param checks_contract	This contract is notified when a badge is created and issued. Notified contract
                              can validate the inputs (which include creator, issued to, issued from) and terminate
                              the transaction if inputs are not in line with org expects.
		* @param producers  This is list of source badge contracts that org recognizes as valid contracts. E.g. of 
                           this is Simple Badge contract and Gotcha Badge Contract.
                          
                          Simple badge contract generates badges which are meant to be issued by a person 
                          of authority in an org.

                          Gotcha badge contract generated badges which are meant to be issued by members to members.

		* @param aacollection This is atomic asset collection name. This can be left blank if org does not wish to use atomic assets
                          related features. Atomic assets feature enables storing badges granularly.
		* @return no return value.
		*/
    ACTION initsystem(name checks_contract, 
      vector<name> producers, 
      name aacollection);
    ACTION chkscontract (name checks_contract);
    /*
		* 
    * Defines - a badge of type simple badge contract 
		*
		* This action creates a simple badge and consumers that the badge has subscribed to. All inputs can be
    * validated in checks_contract ( which is setup in initsystem action )
    * 
		*
		* @param creator	Authorized person to execute this action. 
		* @param badge  Name of the badge. 
		* @param ipfs_image ipfs image associated with the badge.
    * @param display_name Text for display purpose.
    * @param consumers Vector of consumers that a badge has subscribed. Available feature set includes
    *           1) Write to Atomic Asset as Non Transferrable Token. This is recommended to be used
    *               when org wants the badges to be stored granularly. For this to work, org must setup 
    *               a collection using initsystem action OR via initcoll in AA_BADGE_CONTRACT (handy when 
    *               collection is not setup while initialize the system for the org).
    *            2) Cumulative . This enables cumulative maintainence of badge balance for an account.
    *            3) Rounds.  This enables putting a score to the badge. 
		* @return no return value.
		*/

    ACTION initsimple (name creator, 
      name badge, 
      vector<name> parent_badges,
      string offchain_lookup_data, 
      string onchain_lookup_data,
      vector<name> consumers,
      string memo);
    
    
    /*
		* 
    * Defines - a badge of type gotcha badge contract 
		*
		* This action creates a gotcha badge and consumers that the badge has subscribed to. All inputs can be
    * validated in checks_contract ( which is setup in initsystem action )
    *
    * A gotcha badge is issued from member to member. Gotcha badge has two constraints supply_per_cycle and
    * cycle_length. supply_per_cycle is restored at the beginning of new cycle regardless of if member has issued
    * badges (hence reduced available supply for current cycle) to other members or not.
		*
		* @param creator	Authorized person to execute this action. 
		* @param badge  Name of the badge. 
    * @param starttime Time at which badge can be issued from member to member.
    * @param cycle_length Duration after which supply is restored for a member.
    * @param supply_per_cycle Supply per cycle_length
		* @param ipfs_image ipfs image associated with the badge.
    * @param display_name Text for display purpose.
    * @param consumers Vector of consumers that a badge has subscribed. Available feature set includes
    *           1) Write to Atomic Asset as Non Transferrable Token. This is recommended to be used
    *               when org wants the badges to be stored granularly. For this to work, org must setup 
    *               a collection using initsystem action OR via initcoll in AA_BADGE_CONTRACT (handy when 
    *               collection is not setup while initialize the system for the org).
    *            2) Cumulative . This enables cumulative maintainence of badge balance for an account.
    *            3) Rounds.  This enables putting a score to the badge. 
		* @return no return value.
		*/

    ACTION initgotcha (name creator, 
      name badge, 
      time_point_sec starttime, 
      uint64_t cycle_length, 
      uint8_t supply_per_cycle, 
      string offchain_lookup_data, 
      string onchain_lookup_data,
      vector<name> consumers,
      string memo);

    /*
    * Gives a simple badge from authorizer to member
    *
    * @param badge	badge name 
		* @param authorizer  account issuing the badge. authority must be validated in checks_contract 
    * @param to Beneficiary of badge  
    * @param memo Reason for giving the badge
    */  

    ACTION givesimple (name badge, name authorizer, name to, string memo );
    
    /*
    * Gives a gotcha badge from member to member. Inputs must be validated in checks_contract
    *
    * @param badge	badge name 
    * @param from	account issuing the badge 
		* @param to Beneficiary of badge  
    * @param amount 
    * @param memo Reason for giving the badge
    */  

    ACTION givegotcha (name badge, name from, name to, uint8_t amount, string memo );

    
    /*
    * Initialize a new round. In a round, scores for subscriber badged are accumulated from the start status to the end status. 
    *
    * @param authorizor	- authorizer must be validated in checks_contract 
    * @param round -  name of the round 
		* @param display_name - display name of the round.
    */ 
    ACTION initround (name authorizer, name round, string display_name);

    ACTION addbdgetornd (name authorizer,
      name round, 
      uint64_t badge_id, 
      name balance_based_scoring_type, 
      uint16_t balance_based_scoring_weight,
      name source_based_scoring_type,
      uint16_t source_based_scoring_weight);

    ACTION startround (name authorizer, name round);

    ACTION endround (name authorizer, name round);

    ACTION defineseries (name creator, name family);

    ACTION initseriesbdg (name creator, 
      name family, 
      name badge, 
      string offchain_lookup_data, 
      string onchain_lookup_data,
      vector<name> consumers,
      string memo);
    
    ACTION givelatestsb (name issuer, name family, name to, string memo);

  private:

    TABLE settings {
      uint64_t id;
      name checks_contract;
      auto primary_key() const { return id; }
    };
    typedef multi_index<name("settings"), settings> settings_table;

    name checkscontract() {
      settings_table _settings( get_self(), get_self().value );
      auto itr = _settings.require_find(1, "init missing");
      return itr->checks_contract;
    }
    

    struct orchestrator_recognize_args {
      name org;
      name trusted_badge_contract;
    };

    struct orchestrator_addfeature_args {
      name org;
      name badge_contract;
      name badge_name;
      name notify_account;
      string memo;
    };

    struct aa_initcoll_args {
      name org;
      name collection_name;
    };

    struct chkscontract_args {
      name checks_contract;
    };
 
    struct createsimple_args {
      name org;
      name badge;
      vector<name> parent_badges;
      string offchain_lookup_data; 
      string onchain_lookup_data;
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

    struct series_createnext_args {
      name org;
      name family;
      name badge;
      string offchain_lookup_data;
      string onchain_lookup_data;
      string memo;
    };

    struct defineseries_args {
      name org;
      name family;
    };

    struct issue_latestseries_args {
      name org;
      name family;
      name to;
      string memo;
    };

    struct givegotcha_args {
      name org;
      name badge;
      name from;
      name to;
      uint8_t amount;
      string memo;
    };

    struct issuesimple_args {
      name org;
      name to;
      name badge;
      string memo;
    };

    struct createround_args {
      name org;
      name round;
      string description;
      bool account_constrained;
      vector<name> participating_accounts;
    };

    struct startround_args {
      name org;
      name round;
    };

    struct endround_args {
      name org;
      name round;
    };

    struct addscoremeta_args {
      name org;
      name round;
      uint64_t badge_id;
      name balance_based_scoring_type;
      uint16_t balance_based_scoring_weight;
      name source_based_scoring_type;
      uint16_t source_based_scoring_weight;
    };


};
