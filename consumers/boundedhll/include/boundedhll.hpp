#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <hyperloglog.hpp>
#include <eosio/crypto.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT "simplebadgey"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define AUTHORITY_CONTRACT "authorityyyy"
#define BOUNDED_AGG_CONTRACT "baggyyyyyyyy"
#define SUBSCRIPTION_CONTRACT "subyyyyyyyyy"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"

CONTRACT boundedhll : public contract {
public:
    using contract::contract;

    // Notify on new badge issuance
    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]]
    void notifyachiev (
        name org,
        asset badge_asset,
        name from,
        name to,
        string memo,
        vector<name> notify_accounts
    );

    ACTION newemission(
        name org,
        symbol agg_symbol,
        symbol badge_symbol,
        vector<symbol> sender_uniqueness_badge_symbols
    );

    ACTION activate(name org, symbol agg_symbol, symbol badge_symbol);

    ACTION deactivate(name org, symbol agg_symbol, symbol badge_symbol);
private:
    // scoped by agg_symbol
    TABLE emissions {
        symbol badge_symbol;
        vector<symbol> sender_uniqueness_badge_symbols;
        name status;
        auto primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<name("emissions"), emissions> emissions_table;

    // scoped by account
    TABLE balances {
        uint64_t badge_agg_seq_id; // Badge name
        vector<uint8_t> hll;       // HyperLogLog data or any other binary data
        auto primary_key() const { return badge_agg_seq_id; }
    };
    typedef multi_index<"balances"_n, balances> balances_table;

    // scoped by org.
    TABLE badgestatus {
        uint64_t badge_agg_seq_id;  // Primary key: Unique ID for each badge-sequence association
        symbol agg_symbol;          // The aggregation symbol associated with this badge
        uint64_t seq_id;            // The sequence ID this badge is associated with
        symbol badge_symbol;        // The symbol representing the badge
        name badge_status;          // The status of the badge (e.g., "active")
        name seq_status;            // The status of the sequence copied from the sequence table

        uint64_t primary_key() const { return badge_agg_seq_id; }
        uint128_t by_agg_seq() const { return combine_keys(agg_symbol.code().raw(), seq_id); }
        checksum256 by_badge_status() const {
            auto data = badge_symbol.code().to_string() + badge_status.to_string() + seq_status.to_string();
            return sha256(data.data(), data.size());
        }
        checksum256 by_agg_seq_badge() const {
            auto data = agg_symbol.code().to_string() + std::to_string(seq_id) + badge_symbol.code().to_string();
            return sha256(data.data(), data.size());
        }

        static uint128_t combine_keys(uint64_t a, uint64_t b) {
            return (static_cast<uint128_t>(a) << 64) | b;
        }
    };
    typedef eosio::multi_index<"badgestatus"_n, badgestatus,
        indexed_by<"byaggseq"_n, const_mem_fun<badgestatus, uint128_t, &badgestatus::by_agg_seq>>,
        indexed_by<"bybadgestat"_n, const_mem_fun<badgestatus, checksum256, &badgestatus::by_badge_status>>,
        indexed_by<"aggseqbadge"_n, const_mem_fun<badgestatus, checksum256, &badgestatus::by_agg_seq_badge>>
    > badgestatus_table;

    checksum256 hash_active_status(const symbol& badge_symbol, const name& badge_status, const name& seq_status) {
        string data_str = badge_symbol.code().to_string() + badge_status.to_string() + seq_status.to_string();
        return sha256(data_str.data(), data_str.size());
    }

    checksum256 hash_agg_seq_badge(const symbol& agg_symbol, uint64_t seq_id, symbol badge_symbol) {
        string data_str = agg_symbol.code().to_string() + std::to_string(seq_id) + badge_symbol.code().to_string();
        return sha256(data_str.data(), data_str.size());
    }

    TABLE auth {
        name action;
        vector<name> authorized_contracts;
        uint64_t primary_key() const { return action.value; }
    };
    typedef eosio::multi_index<"auth"_n, auth> auth_table;

    void check_internal_auth(name action, string failure_identifier) {
        auth_table _auth(name(AUTHORITY_CONTRACT), _self.value);
        auto itr = _auth.find(action.value);
        check(itr != _auth.end(), failure_identifier + "no entry in authority table for this action and contract");
        for(auto& authorized : itr->authorized_contracts) {
            if(has_auth(authorized)) {
                return;
            }
        }
        check(false, failure_identifier + "Calling contract not in authorized list of accounts for action " + action.to_string());
    }

    struct issue_args {
        name org;
        asset badge_asset;
        name to;
        string memo;
    };

    struct billing_args {
      name org;
      uint8_t actions_used;
    };
};
