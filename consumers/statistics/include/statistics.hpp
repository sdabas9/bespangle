#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#define CUMULATIVE_CONTRACT "cumulativezz"
#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define AUTHORITY_CONTRACT "authorityzzz"
#define ORG_CONTRACT "orgzzzzzzzzz"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"


using namespace std;
using namespace eosio;

CONTRACT statistics : public contract {
  public:
    using contract::contract;

    ACTION settings(name org, symbol badge_symbol, uint64_t max_no_of_ranks);

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts);

  private:
    // scoped by org
    TABLE statssetting {
        symbol badge_symbol;
        uint64_t max_no_of_ranks;
        uint64_t current_no_of_ranks;
        uint64_t primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<"statssetting"_n, statssetting> statssetting_table;

    // scoped by org
    TABLE counts {
        symbol badge_symbol;
        uint64_t total_recipients;
        uint64_t total_issued;
        uint64_t primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<"counts"_n, counts> counts_table;

    // scoped by badge_symbol
    TABLE ranks {
        vector<name> accounts;
        uint64_t balance;
        uint64_t primary_key() const { return balance; }
    };
    typedef multi_index<"ranks"_n, ranks> ranks_table;

    // scoped by recepient
    TABLE account {
        asset    balance;
        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };
    typedef eosio::multi_index<"accounts"_n, account> accounts;

    // scoped by org
    TABLE badge {
        symbol badge_symbol;
        vector<name> notify_accounts;
        string offchain_lookup_data;
        string onchain_lookup_data;
        uint64_t rarity_counts;
        auto primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<name("badge"), badge> badge_table;

    // scoped by contract
    TABLE auth {
      name action;
      vector<name> authorized_contracts;
      uint64_t primary_key() const { return action.value; }
    };
    typedef eosio::multi_index<"auth"_n, auth> auth_table;

    void check_internal_auth (name action, string failure_identifier) {
      auth_table _auth(name(AUTHORITY_CONTRACT), _self.value);
      auto itr = _auth.find(action.value);
      check(itr != _auth.end(), failure_identifier + "no entry in authority table for this action and contract");
      auto authorized_contracts = itr->authorized_contracts;
      for(auto i = 0 ; i < authorized_contracts.size(); i++ ) {
        if(has_auth(authorized_contracts[i])) {
          return;
        }
      }
      check(false, failure_identifier + "Calling contract not in authorized list of accounts for action " + action.to_string());
    }

    void update_rank(name org, name account, symbol badge_symbol, uint64_t old_balance, uint64_t new_balance) {

        ranks_table _ranks(get_self(), badge_symbol.code().raw()); // Use badge_agg_seq_id as scope
        statssetting_table _statssetting(get_self(), org.value);
        auto statssetting_itr = _statssetting.find(badge_symbol.code().raw());
        check(statssetting_itr != _statssetting.end(), "no record in statssetting table");
        
        uint64_t current_no_of_ranks = statssetting_itr->current_no_of_ranks;
        uint64_t max_no_of_ranks = statssetting_itr->max_no_of_ranks;

        bool new_account = false;

        // Check and remove the player's name from the old score, if provided and different

        if(_ranks.begin() == _ranks.end() || new_balance > _ranks.begin()->balance || max_no_of_ranks > current_no_of_ranks) {

            if (old_balance != new_balance) {
                auto old_itr = _ranks.find(old_balance);
                if (old_itr != _ranks.end()) {
                    auto old_names = old_itr->accounts;
                    auto old_size = old_names.size();
                    old_names.erase(std::remove(old_names.begin(), old_names.end(), account), old_names.end());
                    auto new_old_size = old_names.size();
                    if(old_size == new_old_size) {
                        new_account = true;
                    }
                    if (old_names.empty()) {
                        _ranks.erase(old_itr); // Remove the score entry if no names are left
                    } else {
                        _ranks.modify(old_itr, get_self(), [&](auto& entry) {
                            entry.accounts = old_names;
                        });
                    }
                } else {
                    new_account = true;
                }
            }
            auto new_itr = _ranks.find(new_balance);
            if (new_itr == _ranks.end()) {
                _ranks.emplace(get_self(), [&](auto& entry) {
                    entry.balance = new_balance;
                    entry.accounts.push_back(account);
                });
            } else {
                if (std::find(new_itr->accounts.begin(), new_itr->accounts.end(), account) == new_itr->accounts.end()) {
                    _ranks.modify(new_itr, get_self(), [&](auto& entry) {
                        entry.accounts.push_back(account);
                    });
                } // If player name already exists for this score, no action needed
            }
            if(new_account && max_no_of_ranks == current_no_of_ranks) {
                auto rank_itr = _ranks.begin();
                auto lowest_rank_names = rank_itr->accounts;
                lowest_rank_names.pop_back();
                if (lowest_rank_names.empty()) {
                    _ranks.erase(rank_itr); 
                } else {
                    _ranks.modify(rank_itr, get_self(), [&](auto& entry) {
                        entry.accounts = lowest_rank_names;
                    });
                }
            } else if (new_account && max_no_of_ranks > current_no_of_ranks) {
                _statssetting.modify(statssetting_itr, get_self(), [&](auto& entry) {
                    entry.current_no_of_ranks++;
                });
            }
        }
        

    }

};
