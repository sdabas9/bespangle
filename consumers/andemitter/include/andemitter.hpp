#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CONTRACT "simplebadgez"
#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define AUTHORITY_CONTRACT "authorityzzz"
#define ORG_CONTRACT "orgzzzzzzzzz"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"

CONTRACT andemitter : public contract {
public:
    using contract::contract;

    struct contract_asset {
        name contract;
        asset emit_asset;
    };

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev(
        asset amount, 
        name from, 
        name to, 
        string memo, 
        vector<name> notify_accounts
    );

    ACTION newemission(
        name org,
        name emission_name,
        vector<asset> emitter_criteria,
        vector<contract_asset> emit_assets,
        bool cyclic
    );
    
    ACTION chkorgasset(name org, vector<asset> emitter_criteria, vector<contract_asset> emit_assets);
    ACTION activate(name org, name emission_name);
    ACTION deactivate(name org, name emission_name);

private:
    TABLE emissions {
        name emission_name;
        map<symbol_code, asset> emitter_criteria;
        vector<contract_asset> emit_assets;
        name status; // INIT, ACTIVATE, DEACTIVATE
        bool cyclic;
        auto primary_key() const { return emission_name.value; }
    };
    typedef multi_index<name("emissions"), emissions> emissions_table;

    TABLE activelookup {
        symbol badge_symbol;
        vector<name> active_emissions;
        auto primary_key() const { return badge_symbol.code().raw(); }
    };
    typedef multi_index<name("activelookup"), activelookup> activelookup_table;

    TABLE accounts {
        name emission_name;
        uint8_t emission_status;
        map<symbol_code, int64_t> expanded_emitter_status;
        auto primary_key() const { return emission_name.value; }
    };
    typedef multi_index<"accounts"_n, accounts> accounts_table;

    TABLE orgcode {
        name org;
        name org_code;
        auto primary_key() const { return org.value; }
        uint64_t by_org_code() const { return org_code.value; }
    };
    typedef multi_index<"orgcodes"_n, orgcode, indexed_by<"orgcodeidx"_n, const_mem_fun<orgcode, uint64_t, &orgcode::by_org_code>>> orgcode_index;

    void validate_org_assets(name org, const vector<asset>& emitter_criteria, const vector<contract_asset>& emit_assets);

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

    void invoke_action(name to, vector<contract_asset> emit_assets, uint8_t emit_factor);
    
    struct issue_args {
      asset badge_asset;
      name to;
      string memo;
    };
    void update_expanded_emitter_status(name account, map<symbol_code, int64_t>& expanded_emitter_status, const asset& new_asset, emissions emission, uint8_t& emission_status);
};
