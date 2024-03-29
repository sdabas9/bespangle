#include <eosio/eosio.hpp>
#include <hyperloglog.hpp>

using namespace std;
using namespace eosio;

#define BILLING_CONTRACT "billing11111"
#define ORCHESTRATOR_CONTRACT_NAME "router111111"
#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT_NAME"::notifyachiev"

CONTRACT rounds : public contract {
  public:
    using contract::contract;

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


    ACTION createround (name org,
      name round,
      string description,
      bool account_constrained,
      vector<name> participating_accounts);

    ACTION startround (name org, name round);
    ACTION endround (name org, name round);
    ACTION pauseround (name org, name round);
    ACTION resumeround (name org, name round);


    ACTION addscoremeta (name org,
      name round, 
      uint64_t badge_id, 
      name balance_based_scoring_type, 
      uint16_t balance_based_scoring_weight,
      name source_based_scoring_type,
      uint16_t source_based_scoring_weight);


   // low priority ACTION copyround (name sourceround, name targetround) ;


  private:

    TABLE round {
      name round;
      string description;
      bool account_constrained;
      vector<name> participating_accounts;
      name round_status; // init, progress, pause, finished
      auto primary_key() const {return round.value; }
    };
    typedef multi_index<name("round"), round> rounds_table;

    TABLE scoremeta {
      uint64_t scoremeta_id;
      name round;
      uint64_t badge_id;
      name round_status_tentative; // ended, notended
      name balance_based_scoring_type; // linear, fibo
      uint16_t balance_based_scoring_weight; 
      name source_based_scoring_type; // linear, fibo
      uint16_t source_based_scoring_weight;
      name badge_status; // active, pause

      auto primary_key() const {return scoremeta_id; }

      uint128_t badge_round_status_key() const {
        return ((uint128_t) badge_id) << 64 | round_status_tentative.value;
      }
    };
    typedef multi_index<name("scoremeta"), scoremeta,
    indexed_by<name("badgestatus"), const_mem_fun<scoremeta, uint128_t, &scoremeta::badge_round_status_key>>    
    > scoremeta_table; 

    TABLE scores {
      uint64_t score_id;
      uint64_t scoremeta_id;
      name account;
      uint64_t score;
      uint64_t balance_count;
      uint64_t source_count;
      vector<uint8_t> hll;
      auto primary_key() const {return score_id; }

      uint128_t score_account_key() const {
        return ((uint128_t) scoremeta_id) << 64 | account.value;
      }
    };
    typedef multi_index<name("scores"), scores,
    indexed_by<name("scoreaccount"), const_mem_fun<scores, uint128_t, &scores::score_account_key>>    
    > scores_table; 


  uint64_t quad(uint64_t n) {
    return (n*n + n) / 2;
  }

  uint64_t fib(uint64_t n) {
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
  }

/*void balance_based_quadratic_score (name org, uint64_t scoremeta_id, name account, uint8_t count, uint64_t wt) {
  scores_table _scores (get_self, org.value);
    
  auto score_account_index = _scores.get_index<name("scoreaccount")>();
  uint128_t score_account_key = ((uint128_t) scoremeta_id) << 64 | account.value;
  auto score_account_iterator = score_account_index.find (score_account_key);
  

  if (score_account_iterator == score_account_index.end()) {
    uint64_t quad_num = quad (count); // optimize this.
    _scores.emplace(get_self(), [&](auto& row){
      row.id = _scores.available_primary_key();
      row.scoremeta_id = scoremeta_id;
      row.account = account;
      row.score = quad_num * wt;
      row.balance_count = count;
    });  
  }
  else {
    uint64_t quad_num = quad (count + score_account_iterator->balance_count); // optimize this.
    score_account_index.modify(score_account_iterator, get_self, [&](auto& row) {
      row.score = row.score + quad_num * wt;
      row.balance_count = row.balance_count + count;
    });
  }
}*/

  void balance_based_linear_score (name org, uint64_t scoremeta_id, name account, uint8_t count, uint64_t wt) {
    scores_table _scores (get_self(), org.value);
        
  
    auto score_account_index = _scores.get_index<name("scoreaccount")>();
    uint128_t score_account_key = ((uint128_t) scoremeta_id) << 64 | account.value;
    auto score_account_iterator = score_account_index.find (score_account_key);

    if (score_account_iterator == score_account_index.end()) {
      _scores.emplace(get_self(), [&](auto& row){
        row.score_id = _scores.available_primary_key();
        row.scoremeta_id = scoremeta_id;
        row.account = account;
        row.score = count * wt;
        row.balance_count = count;
      }); 
    }
    else {
      score_account_index.modify(score_account_iterator, get_self(), [&](auto& row) {
        row.score = row.score + count * wt;
        row.balance_count = row.balance_count + count;
      });
    }
  }

  void balance_based_fibo_score (name org, uint64_t scoremeta_id, name account, uint8_t count, uint64_t wt) {
    scores_table _scores (get_self(), org.value);
      
    auto score_account_index = _scores.get_index<name("scoreaccount")>();
    uint128_t score_account_key = ((uint128_t) scoremeta_id) << 64 | account.value;
    auto score_account_iterator = score_account_index.find (score_account_key);
    

    if (score_account_iterator == score_account_index.end()) {
      uint64_t fib_num = fib (count); // optimize this.
      _scores.emplace(get_self(), [&](auto& row){
        row.score_id = _scores.available_primary_key();
        row.scoremeta_id = scoremeta_id;
        row.account = account;
        row.score = fib_num * wt;
        row.balance_count = count;
      });  
    }
    else {
      uint64_t fib_num = fib (count + score_account_iterator->balance_count); // optimize this.
      score_account_index.modify(score_account_iterator, get_self(), [&](auto& row) {
        row.score = row.score + fib_num * wt;
        row.balance_count = row.balance_count + count;
      });
    }
  }

  void source_based_linear_score (name org, uint64_t scoremeta_id, name account, name from, uint64_t wt) {
    scores_table _scores (get_self(), org.value);
      
    auto score_account_index = _scores.get_index<name("scoreaccount")>();
    uint128_t score_account_key = ((uint128_t) scoremeta_id) << 64 | account.value;
    auto score_account_iterator = score_account_index.find (score_account_key);


    uint8_t b = 7;
    uint32_t m = 1 << b;
    
    if (score_account_iterator == score_account_index.end()) {
      
      vector<uint8_t> M (m, 0);
      hll::HyperLogLog hll(b, m, M );
      hll.add(from.to_string().c_str(), from.to_string().size());
      
      _scores.emplace(get_self(), [&](auto& row){
        row.score_id = _scores.available_primary_key();
        row.scoremeta_id = scoremeta_id;
        row.account = account;
        row.score = wt;
        row.source_count = 1;
        row.hll = hll.registers();
      });  
    } else if (score_account_iterator->hll.empty()) {
      vector<uint8_t> M (m, 0);
      hll::HyperLogLog hll(b, m, M );
      hll.add(from.to_string().c_str(), from.to_string().size());
      score_account_index.modify(score_account_iterator, get_self(), [&](auto& row) {
        row.score = row.score + wt;
        row.source_count = row.source_count + 1;
        row.hll = hll.registers();
      });

    } else {
      bool increment = false;

      vector<uint8_t> M = score_account_iterator->hll;
      hll::HyperLogLog hll(b, m, M);
      double estimate_before = hll.estimate();
      hll.add(from.to_string().c_str(), from.to_string().size());
      double estimate_after = hll.estimate();

      if(estimate_before != estimate_after) {
        increment = true;
      }
      if(increment) {
        score_account_index.modify(score_account_iterator, get_self(), [&](auto& row) {
          row.score = row.score + wt;
          row.source_count = row.source_count + 1;
          row.hll = hll.registers();
        });
      }
    }

  }

  void source_based_fibo_score (name org, uint64_t scoremeta_id, name account, name from, uint64_t wt) {
    scores_table _scores (get_self(), org.value);
      
    auto score_account_index = _scores.get_index<name("scoreaccount")>();
    uint128_t score_account_key = ((uint128_t) scoremeta_id) << 64 | account.value;
    auto score_account_iterator = score_account_index.find (score_account_key);


    uint8_t b = 7;
    uint32_t m = 1 << b;
    
    if (score_account_iterator == score_account_index.end()) {
      vector<uint8_t> M (m, 0);
      hll::HyperLogLog hll(b, m, M );
      hll.add(from.to_string().c_str(), from.to_string().size());
      
      _scores.emplace(get_self(), [&](auto& row){
        row.score_id = _scores.available_primary_key();
        row.scoremeta_id = scoremeta_id;
        row.account = account;
        row.score = wt;
        row.source_count = 1;
        row.hll = hll.registers();
      });  
    } else if (score_account_iterator->hll.empty()) {
      vector<uint8_t> M (m, 0);
      hll::HyperLogLog hll(b, m, M );
      hll.add(from.to_string().c_str(), from.to_string().size());
      score_account_index.modify(score_account_iterator, get_self(), [&](auto& row) {
        row.score = row.score + wt;
        row.source_count = 1;
        row.hll = hll.registers();
      });

    }
    else {
      bool increment = false;

      vector<uint8_t> M = score_account_iterator->hll;
      hll::HyperLogLog hll(b, m, M);
      double estimate_before = hll.estimate();
      hll.add(from.to_string().c_str(), from.to_string().size());
      double estimate_after = hll.estimate();

      if(estimate_before != estimate_after) {
        increment = true;
      }
      if(increment) {
        uint64_t fib_num = fib (1 + score_account_iterator->source_count); 
        score_account_index.modify(score_account_iterator, get_self(), [&](auto& row) {
          row.score = row.score + fib_num * wt;
          row.source_count = row.source_count + 1;
          row.hll = hll.registers();
        });
      }
    }
  }

};
