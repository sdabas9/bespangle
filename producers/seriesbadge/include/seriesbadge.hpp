#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "orchestrator"
#define NOTIFICATION_CONTRACT_NAME "notification"
#define BILLING_CONTRACT "billing.rep"


#define NEW_SERIES_CREATE_NOTIFICATION NOTIFICATION_CONTRACT_NAME"::createseries"
#define LATEST_BADGE_CREATE_NOTIFICATION NOTIFICATION_CONTRACT_NAME"::createnext"
#define ISSUE_LATEST_BADGE_NOTIFICATION NOTIFICATION_CONTRACT_NAME"::issuelatest"
#define ISSUE_ANY_BADGE_NOTIFICATION NOTIFICATION_CONTRACT_NAME"::issueany"

CONTRACT seriesbadge : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(NEW_SERIES_CREATE_NOTIFICATION)]]
    void extcseries(name org, name series);

    [[eosio::on_notify(LATEST_BADGE_CREATE_NOTIFICATION)]]
    void extcnext(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo);
    
    [[eosio::on_notify(ISSUE_LATEST_BADGE_NOTIFICATION)]]
    void extilatest(name org, name series, name to, uint64_t count, string memo);
    
    [[eosio::on_notify(ISSUE_ANY_BADGE_NOTIFICATION)]]
    void extiany(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo);

    ACTION createseries(name org, name series);
    ACTION createnext(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo);
    ACTION issuelatest(name org, name series, name to, uint64_t count, string memo);
    ACTION issueany(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo);

  private:
    TABLE metadata {
      name series;
      uint64_t seq_id;
      auto primary_key() const {return series.value; }
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

    TABLE badge {
      uint64_t id;
      name series;
      uint64_t seq_id;
      name badge;
      auto primary_key() const {return id; }

      uint128_t series_seq_id_key() const {
        return ((uint128_t) seq_id) << 64 | series.value;
      }
      uint128_t series_badge_key() const {
        return ((uint128_t) series.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("badge"), badge,
    indexed_by<name("seriesseqid"), const_mem_fun<badge, uint128_t, &badge::series_seq_id_key>>,
    indexed_by<name("seriesbadge"), const_mem_fun<badge, uint128_t, &badge::series_badge_key>>
    > badge_table;

    struct createseries_args {
      name org;
      name series;
    };

    struct createnext_args {
      name org;
      name series;
      name badge;
      string offchain_lookup_data;
      string onchain_lookup_data;
      string memo;
    };

    struct issuelatest_args {
      name org;
      name series;
      name to;
      uint64_t count;
      string memo;
    };

    struct issueany_args {
      name org;
      name series;
      uint64_t seq_id;
      name to;
      uint64_t count;
      string memo;
    };

    struct achievement_args {
      name org;
      name badge_name;
      name account;
      name from;
      uint64_t count;
      string memo;
    };

    struct initbadge_args {
      name org;
      name badge_name;
      string offchain_lookup_data; 
      string onchain_lookup_data; 
      string memo;
    }; 

};
