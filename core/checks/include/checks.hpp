#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT_NAME "basicissue11"
#define ORCHESTRATOR_CONTRACT_NAME "router111111"
#define CUMULATIVE_CONTRACT_NAME "accounting11"
#define SERIESBADGE_CONTRACT_NAME "seriesbadge1"

CONTRACT checks : public contract {
  public:
    using contract::contract;

    struct sbadges {
      name badge;
      uint64_t count;
    };

    ACTION ownnotown(name org, name account, vector<sbadges> own, vector<sbadges> not_own);

    ACTION lookback(name org, name account, name series, uint8_t lookback, uint8_t musthave);

    ACTION haslatest(name org, name account, name series);

  private:
    TABLE mbadge {
      uint64_t badge_id;
      name badge_name;
      vector<name> notify_accounts;
      string offchain_lookup_data;
      string onchain_lookup_data;
      uint64_t rarity_counts;
      auto primary_key() const {return badge_id; }
      uint64_t badge_key() const {
        return badge_name.value;
      }
    };
    typedef multi_index<name("badge"), mbadge,
    indexed_by<name("badgename"), const_mem_fun<mbadge, uint64_t, &mbadge::badge_key>>    
    > metadata_badge_table;

    TABLE achievements {
      uint64_t id;
      name account;
      uint64_t badge_id;
      uint64_t count;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badge_id;
      }
    };
    typedef multi_index<name("achievements"), achievements,
    indexed_by<name("accountbadge"), const_mem_fun<achievements, uint128_t, &achievements::acc_badge_key>>
    > achievements_table;

    TABLE metadata {
      name series;
      uint64_t seq_id;
      auto primary_key() const {return series.value; }
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

    TABLE sebadge {
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
    typedef multi_index<name("badge"), sebadge,
    indexed_by<name("seriesseqid"), const_mem_fun<sebadge, uint128_t, &sebadge::series_seq_id_key>>,
    indexed_by<name("seriesbadge"), const_mem_fun<sebadge, uint128_t, &sebadge::series_badge_key>>
    > series_badge_table;

    uint64_t get_badge_id (name org, name assetname) {
      metadata_badge_table _badge(name(ORCHESTRATOR_CONTRACT_NAME), org.value);
      auto badge_index = _badge.get_index<name("badgename")>();
      auto badge_iterator = badge_index.find (assetname.value);
      check(badge_iterator->badge_name == assetname, "asset not found");
      return badge_iterator->badge_id;
    }

    uint64_t account_balance(name org, name account, uint64_t badge_id) {
      achievements_table _achievements(name(CUMULATIVE_CONTRACT_NAME), org.value);
      auto account_badge_index = _achievements.get_index<name("accountbadge")>();
      uint128_t account_badge_key = ((uint128_t) account.value) << 64 | badge_id;
      auto account_badge_iterator = account_badge_index.find(account_badge_key);
      if(account_badge_iterator == account_badge_index.end() || 
        account_badge_iterator->account != account || 
        account_badge_iterator->badge_id != badge_id) {
        return 0;
      } else {
        return account_badge_iterator->count;
      }
    }

    void own_check(name org, name account ,name badge, uint64_t count) {
      uint64_t badge_id = get_badge_id(org,  badge);
      uint64_t balance = account_balance(org, account, badge_id);
      check(balance >= count, "account does not hold enough <badge>. has only <balance>, but needs <count> to pass check");
    }

    void not_own_check(name org, name account ,  name badge, uint64_t count) {
      uint64_t badge_id = get_badge_id(org, badge);
      uint64_t balance = account_balance(org, account, badge_id);
      check(balance < count, "account has balance of <balance> for <badge>. to successfully pass check should hold less than <count>");    
    }

    uint64_t latest_seq_id (name org, name series) {
      metadata_table _metadata(name(SERIESBADGE_CONTRACT_NAME), org.value);
      auto metadata_itr = _metadata.require_find(series.value, "Series not defined");
      return metadata_itr->seq_id;
    }

    uint8_t series_own_count (name org, name account, name series, vector<uint64_t> sequences) {
      uint8_t count = 0;
      series_badge_table _badge(name(SERIESBADGE_CONTRACT_NAME), org.value);
      auto series_seq_id_index = _badge.get_index<name("seriesseqid")>();
      for (auto i = 0 ; i < sequences.size(); i++) {
        
        uint128_t series_seq_id_key = ((uint128_t) sequences[i]) << 64 | series.value;
        auto series_seq_id_iterator = series_seq_id_index.find(series_seq_id_key);
        if(series_seq_id_iterator != series_seq_id_index.end() && 
          series_seq_id_iterator->seq_id == sequences[i] &&
          series_seq_id_iterator->series == series) {
          name badge = series_seq_id_iterator->badge;
          uint64_t badge_id = get_badge_id(org,  badge);
          uint64_t balance = account_balance(org, account, badge_id);
          if(balance > 0) {
            count++;
          }
        } else {
          check (false, "checks contract: somethings wrong");
        }
      }
      return count;
    }
};
