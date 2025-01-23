#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT "simplebadgey"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define AUTHORITY_CONTRACT "authorityyyy"
#define ORG_CONTRACT "organizayyyy"
#define SUBSCRIPTION_CONTRACT "subyyyyyyyyy"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"

CONTRACT andemitter : public contract {
public:
    using contract::contract;

    struct contract_asset {
        name contract;
        asset emit_asset;
    };

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev(
        name org,
        asset amount, 
        name from, 
        name to, 
        string memo, 
        vector<name> notify_accounts
    );

    ACTION newemission(
        name org,
        symbol emission_symbol,
        vector<asset> emitter_criteria,
        vector<contract_asset> emit_assets,
        bool cyclic
    );
    
    ACTION chkorgasset(name org, vector<asset> emitter_criteria, vector<contract_asset> emit_assets);
    ACTION activate(name org, symbol emission_symbol);
    ACTION deactivate(name org, symbol emission_symbol);

private:
    // scoped by andemitter contract
    TABLE emissions {
        symbol emission_symbol;
        map<symbol_code, asset> emitter_criteria;
        vector<contract_asset> emit_assets;
        name status; // INIT, ACTIVATE, DEACTIVATE
        bool cyclic;
        auto primary_key() const { return emission_symbol.code().raw(); }
    };
    typedef multi_index<name("emissions"), emissions> emissions_table;

    // scoped by andemitter contract
    TABLE activelookup {
        symbol badge_symbol;
        vector<symbol> active_emissions;
        auto primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<name("activelookup"), activelookup> activelookup_table;

    // scoped by account
    TABLE accounts {
        symbol emission_symbol;
        uint8_t emission_status;
        map<symbol_code, int64_t> expanded_emitter_status;
        auto primary_key() const { return emission_symbol.code().raw(); }
    };
    typedef multi_index<"accounts"_n, accounts> accounts_table;

    // scoped by org contract
    TABLE orgcode {
        name org;
        name org_code;
        auto primary_key() const { return org.value; }
        uint64_t by_org_code() const { return org_code.value; }
    };
    typedef multi_index<"orgcodes"_n, orgcode, indexed_by<"orgcodeidx"_n, const_mem_fun<orgcode, uint64_t, &orgcode::by_org_code>>> orgcode_index;

    void validate_org_assets(name org, const vector<asset>& emitter_criteria, const vector<contract_asset>& emit_assets, string failure_identifier);
    
    void validate_org_badge_symbol(name org, symbol badge_symbol, string failure_identifier) {
        name badge_org = get_org_from_internal_symbol (badge_symbol, failure_identifier);
        check(org == badge_org, failure_identifier + " org not same as in badge_symbol");
    }
    
    name get_org_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
        string _symbol_str = _symbol.code().to_string(); // Convert symbol to string
        check(_symbol_str.size() >= 4, failure_identifier + "symbol must have at least 4 characters.");

        // Extract the first 4 characters as org_code
        string org_code_str = _symbol_str.substr(0, 4);

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
    // scoped by contract
    TABLE auth {
        name action;
        vector<name> authorized_contracts;
        uint64_t primary_key() const { return action.value; }
    };
    typedef multi_index<"auth"_n, auth> auth_table;

    void check_internal_auth(name action, string failure_identifier);

    enum short_emission_status : uint8_t {
        CYCLIC_IN_PROGRESS = 1,
        NON_CYCLIC_IN_PROGRESS = 2,
        NON_CYCLIC_EMITTED = 3
    };

    void invoke_action(name to, vector<contract_asset> emit_assets, uint8_t emit_factor, string failure_identifier);
    
    struct issue_args {
      name org;
      asset badge_asset;
      name to;
      string memo;
    };
    void update_expanded_emitter_status(name account, map<symbol_code, int64_t>& expanded_emitter_status, const asset& new_asset, emissions emission, uint8_t& emission_status, string failure_identifier);

    struct billing_args {
      name org;
      uint8_t actions_used;
    };

};
