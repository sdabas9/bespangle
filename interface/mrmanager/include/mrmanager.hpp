#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define MUTUAL_RECOGNITION_CONTRACT "mrbadgeyyyyy"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define ORG_CONTRACT "organizayyyy"
#define MUTUAL_RECOGNITION_VALIDATION_CONTRACT "mrvalyyyyyyy"
#define CUMULATIVE_CONTRACT "cumulativeyy"
#define STATISTICS_CONTRACT "statisticsyy"

CONTRACT mrmanager : public contract {
public:
    using contract::contract;

    ACTION init(
        name authorized,
        symbol badge_symbol,
        time_point_sec starttime,
        uint64_t cycle_length,
        uint8_t supply_per_cycle,
        string offchain_lookup_data,
        string onchain_lookup_data,
        bool lifetime_aggregate,
        bool lifetime_stats,
        string memo
    );

    ACTION issue(
        symbol badge_symbol,
        uint64_t amount,
        name from,
        name to,
        string memo
    );

    ACTION starttime(
        name authorized,
        symbol badge_symbol,
        time_point_sec new_starttime
    );

    ACTION cyclelength(
        name authorized,
        symbol badge_symbol,
        uint64_t new_cycle_length
    );

    ACTION cyclesupply(
        name authorized,
        symbol badge_symbol,
        uint8_t new_supply_per_cycle
    );

private:
    TABLE checks {
        name org;
        name checks_contract;

        auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("checks"), checks> checks_table;

    void notify_checks_contract(name org) {
        checks_table _checks(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
        auto itr = _checks.find(org.value);
        if (itr != _checks.end()) {
            require_recipient(itr->checks_contract);
        }
    }

    TABLE orgcode {
        name org;         // Organization identifier, used as primary key
        name org_code;    // Converted org_code, ensuring uniqueness and specific format

        auto primary_key() const { return org.value; }

        uint64_t by_org_code() const { return org_code.value; }
    };

    typedef eosio::multi_index<"orgcodes"_n, orgcode,
        eosio::indexed_by<"orgcodeidx"_n, eosio::const_mem_fun<orgcode, uint64_t, &orgcode::by_org_code>>
    > orgcode_index;

    name get_name_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
        string _symbol_str = _symbol.code().to_string();
        check(_symbol_str.size() == 7, failure_identifier + "Symbol must have at least 7 characters.");
        string _str = _symbol_str.substr(4, 7);

        for (auto& c : _str) {
            c = tolower(c);
        }
        return name(_str);
    }

    name get_org_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
        string _symbol_str = _symbol.code().to_string();
        check(_symbol_str.size() >= 4, failure_identifier + "symbol must have at least 4 characters.");
        string org_code_str = _symbol_str.substr(0, 4);

        for (auto& c : org_code_str) {
            c = tolower(c);
        }
        name org_code = name(org_code_str);
        orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
        auto org_code_itr = orgcodes.get_index<"orgcodeidx"_n>().find(org_code.value);

        check(org_code_itr != orgcodes.get_index<"orgcodeidx"_n>().end(), failure_identifier + "Organization code not found.");
        check(org_code_itr->org_code == org_code, failure_identifier + "Organization code not found.");
        return org_code_itr->org;
    }

    struct addfeature_args {
        name org;
        symbol badge_symbol;
        name notify_account;
        string memo;
    };

    struct create_args {
        name org;
        symbol badge_symbol;
        time_point_sec starttime;
        uint64_t cycle_length;
        uint8_t supply_per_cycle;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct issue_args {
        name org;
        asset badge_asset;
        name from;
        name to;
        string memo;
    };

    struct starttime_args {
        name org;
        symbol badge_symbol;
        time_point_sec new_starttime;
    };

    struct cyclelength_args {
        name org;
        symbol badge_symbol;
        uint64_t new_cycle_length;
    };

    struct cyclesupply_args {
        name org;
        symbol badge_symbol;
        uint8_t new_supply_per_cycle;
    };
};
