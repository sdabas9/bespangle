#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <json.hpp>

using json = nlohmann::json;
using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define AUTHORITY_CONTRACT "authorityzzz"
#define BOUNDED_AGG_CONTRACT "baggzzzzzzzz"
#define CUMULATIVE_CONTRACT "cumulativezz"
#define ORG_CONTRACT "orgzzzzzzzzz"


CONTRACT antibadge : public contract {
public:
    using contract::contract;

    ACTION create(symbol anti_badge_symbol,
        symbol badge_symbol,
        name type,
        string offchain_lookup_data,
        string onchain_lookup_data,
        string memo);

    ACTION createredeem(symbol anti_badge_symbol,
        symbol badge_symbol,
        string offchain_lookup_data,
        string onchain_lookup_data,
        string memo);

    ACTION issue (asset anti_badge_asset, name to, string memo);

    ACTION chkissue (asset anti_badge_asset, name account);

    ACTION chkbndissue (asset anti_badge_asset, name account);

private:
    TABLE badge {
      symbol badge_symbol;
      vector<name> notify_accounts;
      string offchain_lookup_data;
      string onchain_lookup_data;
      uint64_t rarity_counts;
      auto primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<name("badge"), badge> badge_table;

    TABLE account {
      asset    balance;
      uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };
    typedef eosio::multi_index<"accounts"_n, account> accounts;

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

        // Define the structure of the table
    TABLE orgcode {
        name org;         // Organization identifier, used as primary key
        name org_code;    // Converted org_code, ensuring uniqueness and specific format

        // Specify the primary key
        auto primary_key() const { return org.value; }

        // Specify a secondary index for org_code to ensure its uniqueness
        uint64_t by_org_code() const { return org_code.value; }
    };

    // Declare the table
    typedef eosio::multi_index<"orgcodes"_n, orgcode,
      eosio::indexed_by<"orgcodeidx"_n, eosio::const_mem_fun<orgcode, uint64_t, &orgcode::by_org_code>>
    > orgcode_index;

    name get_org_from_badge_symbol(const symbol& badge_symbol, string failure_identifier) {
        string badge_symbol_str = badge_symbol.code().to_string(); // Convert symbol to string
        check(badge_symbol_str.size() >= 4, failure_identifier + "Badge symbol must have at least 4 characters.");

        // Extract the first 4 characters as org_code
        string org_code_str = badge_symbol_str.substr(0, 4);

        for (auto & c: org_code_str) {
            c = tolower(c);
        }
        name org_code = name(org_code_str);

        // Set up the orgcode table and find the org_code
        orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
        auto org_code_itr = orgcodes.get_index<"orgcodeidx"_n>().find(org_code.value);

        check(org_code_itr != orgcodes.get_index<"orgcodeidx"_n>().end(), failure_identifier + "Organization code not found.");
        check(org_code_itr->org_code == org_code, failure_identifier + "Organization code not found.");
        // Assuming the org is stored in the same row as the org_code
        return org_code_itr->org; // Return the found organization identifier
    }

    symbol get_badge_for_antibadge (name org, symbol anti_badge_symbol) {
      badge_table _badge(name(ORCHESTRATOR_CONTRACT), org.value);
      auto badge_iterator = _badge.find (anti_badge_symbol.code().raw());
      check(badge_iterator != _badge.end(), "antibadge not created"); 
      auto onchain_lookup_data = json::parse(badge_iterator->onchain_lookup_data);
      string badge_name = onchain_lookup_data["anti_to"];
      symbol_code sym_code(badge_name);
      symbol new_sym(badge_name, 0);
      return new_sym;
    }

    json anti_badges (name org, symbol badge_symbol) {
      badge_table _badge(name(ORCHESTRATOR_CONTRACT), org.value);
      auto badge_iterator = _badge.find (badge_symbol.code().raw());
      check(badge_iterator != _badge.end(), "badge not created"); 
      auto onchain_lookup_data = json::parse(badge_iterator->onchain_lookup_data);
      json entries = onchain_lookup_data["antibadge"];
      return entries;
    }

    vector<symbol> anti_badge_vector (name org, symbol badge_symbol) {
      badge_table _badge(name(ORCHESTRATOR_CONTRACT), org.value);
      auto badge_iterator = _badge.find (badge_symbol.code().raw());
      check(badge_iterator != _badge.end(), "badge not created"); 
      auto onchain_lookup_data = json::parse(badge_iterator->onchain_lookup_data);
      json entries = onchain_lookup_data["antibadge"];
      vector<symbol> anti_badges;
      for (const auto& entry : entries) {
        string badge_name = entry["badge"];
        symbol_code sym_code(badge_name);
        symbol new_sym(badge_name, 0);
        anti_badges.push_back(new_sym);
      }
      return anti_badges;
    }

    uint64_t multiple_badges_additive_balance (vector<symbol> badge_symbols, name account) {
      uint64_t balance = 0;
      for (auto i = 0 ; i < badge_symbols.size(); i++) {
        balance = balance + account_balance(badge_symbols[i], account);    
      }
      return balance;
    }

    uint64_t account_balance(symbol badge_symbol, name account) {
      accounts _accounts(name(CUMULATIVE_CONTRACT), account.value);
      auto itr = _accounts.find(badge_symbol.code().raw());
      return (itr == _accounts.end())? 0 : itr->balance.amount;
    }
    
    uint64_t bounded_account_balance () {
    
    }

    struct create_args {
        symbol anti_badge_symbol;
        symbol badge_symbol;
        name type;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct createinv_args {
        symbol anti_badge_symbol;
        symbol badge_symbol;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct issue_args {
        asset anti_badge_asset;
        name to; 
        string memo;
    };

    struct achievement_args {
        asset badge_asset;
        name from;
        name to;
        string memo;
    };

    struct initbadge_args {
        symbol badge_symbol;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct mergeinfo_args {
        symbol badge_symbol;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct chkissue_args {
        asset badge_asset;
        name account;
    };
};
