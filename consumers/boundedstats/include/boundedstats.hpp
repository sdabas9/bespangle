#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define BOUNDED_AGG_CONTRACT "baggyyyyyyyy"
#define AUTHORITY_CONTRACT "authorityyyy"
#define ORG_CONTRACT "organizayyyy"
#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"

CONTRACT boundedstats : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev(
        name org,
        asset badge_asset, 
        name from, 
        name to, 
        string memo, 
        vector<name> notify_accounts);


    ACTION activate(name org, symbol agg_symbol, vector<symbol> badge_symbols);

    ACTION deactivate(name org, symbol agg_symbol, vector<symbol> badge_symbols);

  private:
    //scoped by agg_symbol
    TABLE  statssetting {
        symbol badge_symbol;
        uint64_t primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<"statssetting"_n, statssetting> statssetting_table;

    // scoped by org
    TABLE counts {
        uint64_t badge_agg_seq_id;
        uint64_t total_recipients;
        uint64_t total_issued;
        uint64_t primary_key() const { return badge_agg_seq_id; }
    };
    typedef multi_index<"counts"_n, counts> counts_table;

    // scoped by badge_agg_seq_id
    TABLE ranks {
        vector<name> accounts;
        uint64_t balance;
        uint64_t primary_key() const { return balance; }
    };
    typedef multi_index<"ranks"_n, ranks> ranks_table;

    // scoped by org.
    TABLE badgestatus {
        uint64_t badge_agg_seq_id; // Primary key: Unique ID for each badge-sequence association
        symbol agg_symbol;         // The aggregation symbol associated with this badge
        uint64_t seq_id;           // The sequence ID this badge is associated with
        symbol badge_symbol;       // The symbol representing the badge
        name badge_status;         // The status of the badge (e.g., "active")
        name seq_status;           // The status of the sequence copied from the sequence table

        uint64_t primary_key() const { return badge_agg_seq_id; }
        uint128_t by_agg_seq() const { return combine_keys(agg_symbol.code().raw(), seq_id); }

        checksum256 by_badge_status() const {
            // Example hashing for badge status; adjust according to actual implementation needs
            auto data = badge_symbol.code().to_string() + badge_status.to_string() + seq_status.to_string();
            return sha256(data.data(), data.size());
        }

        checksum256 by_agg_seq_badge() const {
            // Example hashing for badge status; adjust according to actual implementation needs
            auto data = agg_symbol.code().to_string() + std::to_string(seq_id) + badge_symbol.code().to_string();
            return sha256(data.data(), data.size());
        }

        static uint128_t combine_keys(uint64_t a, uint64_t b) {
            // Combines two 64-bit integers into a single 128-bit value for indexing purposes
            return (static_cast<uint128_t>(a) << 64) | b;
        }
    };
    typedef eosio::multi_index<"badgestatus"_n, badgestatus,
        eosio::indexed_by<"byaggseq"_n, eosio::const_mem_fun<badgestatus, uint128_t, &badgestatus::by_agg_seq>>,
        eosio::indexed_by<"bybadgestat"_n, eosio::const_mem_fun<badgestatus, checksum256, &badgestatus::by_badge_status>>,
        eosio::indexed_by<"aggseqbadge"_n, eosio::const_mem_fun<badgestatus, checksum256, &badgestatus::by_agg_seq_badge>>
    > badgestatus_table;

    checksum256 hash_active_status(const symbol& badge_symbol, const name& badge_status, const name& seq_status) {
        // Create a data string from badge_symbol, badge_status, and seq_status
        string data_str = badge_symbol.code().to_string() + badge_status.to_string() + seq_status.to_string();

        // Return the sha256 hash of the concatenated string
        return sha256(data_str.data(), data_str.size());
    }

    checksum256 hash_agg_seq_badge(const symbol& agg_symbol, uint64_t seq_id, symbol badge_symbol) {
        // Create a data string from badge_symbol, badge_status, and seq_status
        string data_str = agg_symbol.code().to_string() + std::to_string(seq_id) + badge_symbol.code().to_string();

        // Return the sha256 hash of the concatenated string
        return sha256(data_str.data(), data_str.size());
    }

    // scoped by account
    TABLE achievements {
        uint64_t badge_agg_seq_id;
        uint64_t count;

        uint64_t primary_key() const { return badge_agg_seq_id; }
    };
    typedef eosio::multi_index<"achievements"_n, achievements> achievements_table;

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

    void update_rank(name org, name account, uint64_t badge_agg_seq_id, uint64_t old_balance, uint64_t new_balance) {

        ranks_table _ranks(get_self(), badge_agg_seq_id); // Use badge_agg_seq_id as scope
        if (old_balance != new_balance && old_balance != 0) {
            auto old_itr = _ranks.find(old_balance);
            check(old_itr != _ranks.end(),"not found in boundedstats");
            auto old_names = old_itr->accounts;
            old_names.erase(std::remove(old_names.begin(), old_names.end(), account), old_names.end());
            if (old_names.empty()) {
                _ranks.erase(old_itr); // Remove the score entry if no names are left
            } else {
                _ranks.modify(old_itr, get_self(), [&](auto& entry) {
                    entry.accounts = old_names;
                });
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

    }

    void update_count(name org, name account, uint64_t badge_agg_seq_id, uint64_t old_balance, uint64_t new_balance) {
        counts_table _counts(get_self(), org.value);
        auto counts_itr = _counts.find(badge_agg_seq_id);
        uint64_t total_recipients;
        uint64_t total_issued;
        if(counts_itr == _counts.end()) {
            total_recipients = 1;
            total_issued = new_balance;
            _counts.emplace(get_self(), [&](auto& entry) {
                entry.badge_agg_seq_id = badge_agg_seq_id;
                entry.total_recipients = total_recipients;
                entry.total_issued = total_issued;
            });
        } else {
            total_recipients = counts_itr->total_recipients;
            if(old_balance == 0) {
                total_recipients++;
            }
            total_issued = counts_itr->total_issued + new_balance - old_balance;
            _counts.modify(counts_itr, get_self(), [&](auto& entry) {
                entry.total_recipients = total_recipients;
                entry.total_issued = total_issued;
            });
        }
    }

    // Function to fetch the new balance from the achievements table, scoped by account.
    uint64_t get_new_balance(name account, uint64_t badge_agg_seq_id) {
        // Access the achievements table with the account as the scope.
        achievements_table achievements(name(BOUNDED_AGG_CONTRACT), account.value);
        auto achv_itr = achievements.find(badge_agg_seq_id);
        
        // Check if the record exists and return the balance.
        if (achv_itr != achievements.end()) {
            return achv_itr->count; // Assuming 'balance' is the field storing the balance.
        } else {
            // Handle the case where there is no such achievement record.
            eosio::check(false, "Achievement record not found for the given badge_agg_seq_id.");
            return 0; // This return is formal as the check above will halt execution if triggered.
        }
    }
    

};
