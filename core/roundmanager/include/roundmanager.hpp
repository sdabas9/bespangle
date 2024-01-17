#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORG_CHECKS_CONTRACT_NAME "interface111"
#define NOTIFICATION_CONTRACT_NAME "notification"
#define BOUNDED_AGG_CONTRACT_NAME "boundedaggxx"
#define ROUND_MANAGER_VALIDATION_CONTRACT "asdfd"
#define ROUND_DEFERRED_CONTRACT "rounddeferr"
#define ORCHESTRATOR_CONTRACT "orgrgrr"

CONTRACT roundmanager : public contract {
  public:
    using contract::contract;

    struct redeemable {
      name redeem_badge;
      name reward_yielding_badge;
      string offchain_lookup_data;
      string onchain_lookup_data;
      name type;
      uint64_t rate;
      name reward_pool_contract;
    };


ACTION defineround(name org, 
  name authorized_account, 
  name round,
  string round_description, 
  vector<name> badges, 
  vector<redeemable> redeem_badge_info,
  time_point round_start_time,
  time_point round_end_time,
  time_point_sec claim_start_time,
  time_point_sec claim_end_time
  );


ACTION addround(name org, name round, string description);

ACTION setstatus(name org, name round, name status, time_point execution_time);

ACTION addbadge(name org, name round, name badge, time_point execution_time);

ACTION rembadge(name org, name round, name badge, time_point execution_time);

ACTION createredeem(name org,
               name antibadge,
               name badge,
               name type,
               string offchain_lookup_data,
               string onchain_lookup_data,
               string memo);

ACTION initfixed(name org, name round, name badge, uint64_t rate, name notification_contract);

ACTION initdynamic(name org, name round, name badge, uint64_t amount, name notification_contract);

ACTION addfeature(name org, name badge_name, name notify_account, string memo);


    struct addround_args {
      name org;
      name round;
      string description;
    };

    struct setstatus_args {
      name org;
      name round;
      name status;
      time_point execution_time;
    };

    struct addbadge_args {
      name org;
      name round;
      name badge;
      time_point execution_time;
    };

    struct rembadge_args {
      name org;
      name round;
      name badge;
      time_point execution_time;
    };
    
    struct createredeem_args {
      name org;
      name antibadge;
      name badge;
      name type;
      string offchain_lookup_data;
      string onchain_lookup_data;
      string memo;
    };

    struct initfixed_args {
      name org;
      name round;
      name badge;
      uint64_t rate;
      name notification_contract;
    };

    struct initdynamic_args {
      name org;
      name round;
      name badge;
      uint64_t amount;
      name notification_contract;
    };

    struct addfeature_args {
      name org;
      name badge;
      name notify_account;
      string memo;
    };


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
};
