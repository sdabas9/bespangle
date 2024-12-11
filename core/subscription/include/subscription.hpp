#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/time.hpp>
#include <vector>

using namespace eosio;
using namespace std;

#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define BOUNDED_AGG_CONTRACT "baggyyyyyyyy"
#define ANDEMITTER_CONTRACT "andemitteryy"

#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"
#define SERIES_START_NOTIFICATION BOUNDED_AGG_CONTRACT"::actseq"
#define SERIES_END_NOTIFICATION BOUNDED_AGG_CONTRACT"::endseq"
#define START_EMISSION_NOTIFICATION ANDEMITTER_CONTRACT "::activate"
#define END_EMISSION_NOTIFICATION ANDEMITTER_CONTRACT "::deactivate"

CONTRACT subscription : public contract {
public:
    using contract::contract;

    // Notification handlers
    [[eosio::on_notify(SERIES_START_NOTIFICATION)]]
    void startseries(name org, symbol agg_symbol, vector<uint64_t> seq_ids);

    [[eosio::on_notify(SERIES_END_NOTIFICATION)]]
    void endseries(name org, symbol agg_symbol, vector<uint64_t> seq_ids);

    [[eosio::on_notify(START_EMISSION_NOTIFICATION)]]
    void newemission(name org, symbol emission_symbol);

    [[eosio::on_notify(END_EMISSION_NOTIFICATION)]]
    void endemission(name org, symbol emission_symbol);

    [[eosio::on_notify("*::transfer")]]
    void buypack(name from, name to, asset amount, string memo);

    [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]]
    void notifyachiev(
        name org,
        asset badge_asset,
        name from,
        name to,
        string memo,
        vector<name> notify_accounts
    );

    // Actions
    ACTION newpack(
        name package,
        string descriptive_name,
        uint64_t action_size,
        uint16_t max_active_series,
        uint16_t max_active_emissions,
        uint64_t expiry_duration_in_secs,
        extended_asset cost,
        bool active,
        bool display
    );

    ACTION disablepack(name package);

    ACTION enablepack(name package);

    ACTION setsercost(extended_asset additional_series_cost);

    ACTION setemitcost(extended_asset additional_emission_cost);

    ACTION haspackage(name org);

    ACTION uidisplay(name package, bool display);

private:
    // Table definitions
    TABLE sequences {
        name key;
        uint64_t last_used_value;

        uint64_t primary_key() const { return key.value; }
    };
    typedef eosio::multi_index<"sequences"_n, sequences> sequences_table;

    TABLE packages {
        name package;
        string descriptive_name;
        uint64_t action_size;
        uint16_t max_active_series;
        uint16_t max_active_emissions;
        uint64_t expiry_duration_in_secs;
        extended_asset cost;
        bool active;
        bool display;

        uint64_t primary_key() const { return package.value; }
    };
    typedef eosio::multi_index<"packages"_n, packages> packages_table;

    TABLE orgpackage {
        uint64_t seq_id;
        name package;
        name status; // NEW, CURRENT, EXPIRED
        uint64_t total_actions_bought;
        uint64_t actions_used;
        uint16_t max_active_series_available;
        uint16_t current_active_series;
        uint16_t max_active_emissions;
        uint16_t current_active_emissions;
        uint64_t expiry_duration_in_secs;
        time_point_sec expiry_time;

        uint64_t primary_key() const { return seq_id; }
        uint64_t by_status() const { return status.value; }
    };
    typedef eosio::multi_index<
        "orgpackage"_n, orgpackage,
        indexed_by<"bystatus"_n, const_mem_fun<orgpackage, uint64_t, &orgpackage::by_status>>
    > orgpackage_table;

    TABLE seriescost {
        extended_asset additional_series_cost;

        uint64_t primary_key() const {
            return additional_series_cost.quantity.symbol.code().raw();
        }
    };
    typedef eosio::multi_index<"seriescost"_n, seriescost> seriescost_table;

    TABLE emissioncost {
        extended_asset additional_emission_cost;

        uint64_t primary_key() const {
            return additional_emission_cost.quantity.symbol.code().raw();
        }
    };
    typedef eosio::multi_index<"emissioncost"_n, emissioncost> emissioncost_table;

    // Utility functions
    std::tuple<name, std::string, std::string> parse_memo(const std::string& memo) {
        // Split the memo into parts based on the colon delimiter
        std::vector<std::string> parts;
        size_t start = 0;
        size_t end = memo.find(':');
        while (end != std::string::npos) {
            parts.push_back(memo.substr(start, end - start));
            start = end + 1;
            end = memo.find(':', start);
        }
        parts.push_back(memo.substr(start)); // Add the last part

        // Validate the number of fields
        check(parts.size() == 3 || parts.size() == 4, "Invalid memo format: must have 3 or 4 fields");

        // Extract fields
        auto org = name(parts[0]);
        std::string type;
        std::string data;

        if (parts.size() == 3) {
            // For 3-field memo (e.g., <org>:pack:<packagename>)
            type = parts[1];
            data = parts[2];
        } else {
            // For 4-field memo (e.g., <org>:emission:new:<count>)
            type = parts[1] + ":" + parts[2]; // Combine second and third fields for type
            data = parts[3];
        }

        // Validate the type
        check(
            type == "pack" || 
            type == "emission:new" || 
            type == "emission:current" || 
            type == "series:new" || 
            type == "series:current",
            "Invalid memo type"
        );

        // Ensure the data field is non-empty
        check(!data.empty(), "Data field cannot be empty");

        return {org, type, data};
    }


    void refund_surplus(name to, asset surplus, name token_contract) {
        action(
            permission_level{get_self(), "active"_n},
            token_contract,
            "transfer"_n,
            std::make_tuple(get_self(), to, surplus, std::string("Surplus refund"))
        ).send();
    }

    void handle_pack_purchase(orgpackage_table& orgpackages, const std::string& data, asset amount, name from, name first_receiver) {
        auto package_name = name(data);

        // Ensure only one NEW status record exists
        auto status_index = orgpackages.get_index<"bystatus"_n>();
        auto existing_new = status_index.find(name("new").value);
        check(existing_new == status_index.end(), "Another NEW status record already exists");

        // Lookup package in the packages table
        packages_table packages(get_self(), get_self().value);
        auto package_itr = packages.find(package_name.value);
        check(package_itr != packages.end(), "Package not found");
        check(package_itr->cost.quantity.symbol == amount.symbol, "Currency mismatch");
        check(package_itr->cost.contract == first_receiver, "Payment contract does not match package contract");
        check(amount >= package_itr->cost.quantity, "Insufficient funds");

        // Refund surplus
        if (amount > package_itr->cost.quantity) {
            refund_surplus(from, amount - package_itr->cost.quantity, first_receiver);
        }

        // Get the next sequence ID for this package
        uint64_t seq_id = get_next_sequence(package_name);

        // Insert new record in orgpackage table
        orgpackages.emplace(get_self(), [&](auto& row) {
            row.seq_id = seq_id;
            row.package = package_name;
            row.status = "new"_n;
            row.total_actions_bought = package_itr->action_size;
            row.actions_used = 0;
            row.max_active_series_available = package_itr->max_active_series;
            row.current_active_series = 0;
            row.max_active_emissions = package_itr->max_active_emissions;
            row.current_active_emissions = 0;
            row.expiry_duration_in_secs = package_itr->expiry_duration_in_secs;
        });
    }

    void handle_series_update(orgpackage_table& orgpackages, std::string count_str, const std::string& status, asset amount, name from, name first_receiver) {
        int count = std::stoi(count_str);
        auto status_index = orgpackages.get_index<"bystatus"_n>();
        auto itr = status_index.find(name(status).value);
        check(itr != status_index.end(), "No record with status " + status + " exists");

        seriescost_table seriescosts(get_self(), get_self().value);
        auto cost_itr = seriescosts.find(amount.symbol.code().raw());
        check(cost_itr != seriescosts.end(), "Series cost not found");
        check(cost_itr->additional_series_cost.contract == first_receiver, "Payment contract does not match series cost contract");

        auto required_cost = cost_itr->additional_series_cost.quantity * count;
        check(amount >= required_cost, "Insufficient funds");

        if (amount > required_cost) {
            refund_surplus(from, amount - required_cost, first_receiver);
        }

        status_index.modify(itr, get_self(), [&](auto& row) {
            row.max_active_series_available += count;
        });
    }

    void handle_emission_update(orgpackage_table& orgpackages, std::string count_str, const std::string& status, asset amount, name from, name first_receiver) {
        int count = std::stoi(count_str);
        auto status_index = orgpackages.get_index<"bystatus"_n>();
        auto itr = status_index.find(name(status).value);
        check(itr != status_index.end(), "No record with status " + status + " exists");

        emissioncost_table emissioncosts(get_self(), get_self().value);
        auto cost_itr = emissioncosts.find(amount.symbol.code().raw());
        check(cost_itr != emissioncosts.end(), "Emission cost not found");
        check(cost_itr->additional_emission_cost.contract == first_receiver, "Payment contract does not match emission cost contract");

        auto required_cost = cost_itr->additional_emission_cost.quantity * count;
        check(amount >= required_cost, "Insufficient funds");

        if (amount > required_cost) {
            refund_surplus(from, amount - required_cost, first_receiver);
        }

        status_index.modify(itr, get_self(), [&](auto& row) {
            row.max_active_emissions += count;
        });
    }

    uint64_t get_next_sequence(name key) {
        sequences_table sequences(get_self(), get_self().value);

        auto itr = sequences.find(key.value);
        if (itr == sequences.end()) {
            // Initialize sequence if it does not exist
            sequences.emplace(get_self(), [&](auto& row) {
                row.key = key;
                row.last_used_value = 1;
            });
            return 1;
        } else {
            // Increment and return the next sequence value
            sequences.modify(itr, get_self(), [&](auto& row) {
                row.last_used_value += 1;
            });
            return itr->last_used_value;
        }
    }
};
