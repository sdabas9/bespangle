#include <subscription.hpp>

void subscription::startseries(name org, symbol agg_symbol, vector<uint64_t> seq_ids) {
    // Ensure the sequence IDs are provided
    check(!seq_ids.empty(), "Sequence IDs cannot be empty");

    // Access the orgpackage table scoped by the organization
    orgpackage_table orgpackages(get_self(), org.value);

    // Access the record with the "current" status
    auto status_index = orgpackages.get_index<"bystatus"_n>();
    auto current_itr = status_index.find("current"_n.value);

    if (current_itr != status_index.end() && current_itr->status == name("current")) {
        // Ensure the updated active series count does not exceed max allowed
        uint16_t new_active_series = current_itr->current_active_series + seq_ids.size();
        check(new_active_series <= current_itr->max_active_series_available,
              "Exceeded maximum active series allowed in the current package");

        // Update the current_active_series
        status_index.modify(current_itr, get_self(), [&](auto& pkg) {
            pkg.current_active_series = new_active_series;
        });
    } else {
        // Handle the case where there is no record with "current" status
        auto new_itr = status_index.find("new"_n.value);
        check(new_itr != status_index.end() && new_itr->status == name("new"),
              "Neither CURRENT nor NEW package available");

        // Ensure the updated active series count does not exceed max allowed
        uint16_t new_active_series = new_itr->current_active_series + seq_ids.size();
        check(new_active_series <= new_itr->max_active_series_available,
              "Exceeded maximum active series allowed in the new package");

        // Update the current_active_series
        status_index.modify(new_itr, get_self(), [&](auto& pkg) {
            pkg.current_active_series = new_active_series;
        });
    }
}

void subscription::endseries(name org, symbol agg_symbol, vector<uint64_t> seq_ids) {
    // Ensure the sequence IDs are provided
    check(!seq_ids.empty(), "Sequence IDs cannot be empty");

    // Access the orgpackage table scoped by the organization
    orgpackage_table orgpackages(get_self(), org.value);

    // Access the record with the "current" status
    auto status_index = orgpackages.get_index<"bystatus"_n>();
    auto current_itr = status_index.find("current"_n.value);

    if (current_itr != status_index.end() && current_itr->status == name("current")) {
        // Ensure the updated active series count does not go negative
        check(current_itr->current_active_series >= seq_ids.size(),
              "Cannot end more series than active in the current package");

        // Update the current_active_series
        status_index.modify(current_itr, get_self(), [&](auto& pkg) {
            pkg.current_active_series -= seq_ids.size();
        });
    } else {
        // Handle the case where there is no record with "current" status
        auto new_itr = status_index.find("new"_n.value);
        check(new_itr != status_index.end() && new_itr->status == name("new"),
              "Neither current nor new package available");

        // Ensure the updated active series count does not go negative
        check(new_itr->current_active_series >= seq_ids.size(),
              "Cannot end more series than active in the new package");

        // Update the current_active_series
        status_index.modify(new_itr, get_self(), [&](auto& pkg) {
            pkg.current_active_series -= seq_ids.size();
        });
    }
}

void subscription::newemission(name org, symbol emission_symbol) {
    // Access the orgpackage table scoped by the organization
    orgpackage_table orgpackages(get_self(), org.value);

    // Access the record with the "current" status
    auto status_index = orgpackages.get_index<"bystatus"_n>();
    auto current_itr = status_index.find("current"_n.value);

    if (current_itr != status_index.end() && current_itr->status == name("current")) {
        // Ensure the updated active emissions count does not exceed max allowed
        check(current_itr->current_active_emissions + 1 <= current_itr->max_active_emissions,
              "Exceeded maximum active emissions allowed in the current package");

        // Update the current_active_emissions
        status_index.modify(current_itr, get_self(), [&](auto& pkg) {
            pkg.current_active_emissions += 1;
        });
    } else {
        // Handle the case where there is no record with "current" status
        auto new_itr = status_index.find("new"_n.value);
        check(new_itr != status_index.end() && new_itr->status == name("new"), "Neither CURRENT nor NEW package available");

        // Ensure the updated active emissions count does not exceed max allowed
        check(new_itr->current_active_emissions + 1 <= new_itr->max_active_emissions,
              "Exceeded maximum active emissions allowed in the new package");

        // Update the current_active_emissions
        status_index.modify(new_itr, get_self(), [&](auto& pkg) {
            pkg.current_active_emissions += 1;
        });
    }
}

void subscription::endemission(name org, symbol emission_symbol) {
    // Access the orgpackage table scoped by the organization
    orgpackage_table orgpackages(get_self(), org.value);

    // Access the record with the "current" status
    auto status_index = orgpackages.get_index<"bystatus"_n>();
    auto current_itr = status_index.find("current"_n.value);

    if (current_itr != status_index.end() && current_itr->status == name("current")) {
        // Ensure the updated active emissions count does not go below zero
        check(current_itr->current_active_emissions > 0,
              "Cannot end emission: no active emissions available in the current package");

        // Update the current_active_emissions
        status_index.modify(current_itr, get_self(), [&](auto& pkg) {
            pkg.current_active_emissions -= 1;
        });
    } else {
        // Handle the case where there is no record with "current" status
        auto new_itr = status_index.find("new"_n.value);
        check(new_itr != status_index.end() && new_itr->status == name("new"), "Neither CURRENT nor NEW package available");

        // Ensure the updated active emissions count does not go below zero
        check(new_itr->current_active_emissions > 0,
              "Cannot end emission: no active emissions available in the new package");

        // Update the current_active_emissions
        status_index.modify(new_itr, get_self(), [&](auto& pkg) {
            pkg.current_active_emissions -= 1;
        });
    }
}


void subscription::notifyachiev(
    name org,
    asset badge_asset,
    name from,
    name to,
    string memo,
    vector<name> notify_accounts) {
    // Open the orgpackage table scoped by the organization
    orgpackage_table orgpackages(get_self(), org.value);

    // Access the current package
    auto status_index = orgpackages.get_index<"bystatus"_n>();
    auto current_itr = status_index.find(name("current").value);
    time_point_sec current_time = eosio::current_time_point();
    // Check if a current package does not exist, is expired, or is exhausted
    if (current_itr == status_index.end() ||
        current_time > current_itr->expiry_time ||
        current_itr->actions_used >= current_itr->total_actions_bought ||
        current_itr->status != name("current")) {
        // Handle expired or missing current package
        auto new_itr = status_index.find(name("new").value);
        check(new_itr != status_index.end(), "No NEW package available to update");

        // Validate series and emissions limits
        if (current_itr != status_index.end() && current_itr->status == name("current")) {
            check(current_itr->current_active_series <= new_itr->max_active_series_available,
                  "Current active series exceeds max active series available in the new package");
            check(current_itr->current_active_emissions <= new_itr->max_active_emissions,
                  "Current active emissions exceed max active emissions available in the new package");
        }

        // Promote the NEW package to CURRENT
        status_index.modify(new_itr, get_self(), [&](auto& new_pkg) {
            new_pkg.status = "current"_n;
            new_pkg.actions_used = 1; // Start with 1 action used
            new_pkg.expiry_time = time_point_sec(current_time_point().sec_since_epoch() +
                                                 new_pkg.expiry_duration_in_secs);

            // Carry over series and emissions values if there was a previous current package
            if (current_itr != status_index.end()) {
                new_pkg.current_active_series = current_itr->current_active_series;
                new_pkg.current_active_emissions = current_itr->current_active_emissions;
            }
        });

        // Update the old current package to USED if it exists
        if (current_itr != status_index.end() && current_itr->status == name("current")) {
            status_index.modify(current_itr, get_self(), [&](auto& old_pkg) {
                old_pkg.status = "used"_n;
            });
        }
    } else {
        // Increment actions used for the current package
        status_index.modify(current_itr, get_self(), [&](auto& pkg) {
            pkg.actions_used += 1;
        });
    }
}

void subscription::buypack(name from, name to, asset amount, std::string memo) {
    if (from == get_self() || to != get_self()) return;

    check(amount.amount > 0, "Transfer amount must be positive");

    auto [org, type, data] = parse_memo(memo);

    orgpackage_table orgpackages(get_self(), org.value);

    if (type == "pack") {
        handle_pack_purchase(orgpackages, data, amount, from, get_first_receiver());
    } else if (type == "series:current") {
        handle_series_update(orgpackages, data, "current", amount, from, get_first_receiver());
    } else if (type == "series:new") {
        handle_series_update(orgpackages, data, "new", amount, from, get_first_receiver());
    } else if (type == "emission:current") {
        handle_emission_update(orgpackages, data, "current", amount, from, get_first_receiver());
    } else if (type == "emission:new") {
        handle_emission_update(orgpackages, data, "new", amount, from, get_first_receiver());
    } else {
        check(false, "Invalid memo type");
    }
}

ACTION subscription::newpack(
    name package,
    string descriptive_name,
    uint64_t action_size,
    uint16_t max_active_series,
    uint16_t max_active_emissions,
    uint64_t expiry_duration_in_secs,
    extended_asset cost,
    bool active) {
    require_auth(get_self()); // Ensure action is authorized by the contract account

    // Validate inputs
    check(package.length() > 0, "Package name cannot be empty");
    check(descriptive_name.size() > 0, "Descriptive name cannot be empty");
    check(action_size > 0, "Action size must be greater than zero");
    check(expiry_duration_in_secs > 0, "Expiry duration must be greater than zero");
    check(cost.quantity.amount >= 0, "Cost cannot have a negative amount");

    // Access the table
    packages_table _packages(get_self(), get_self().value);

    // Ensure the package name does not already exist
    auto itr = _packages.find(package.value);
    check(itr == _packages.end(), "Package with this name already exists");

    // Insert new record into the table
    _packages.emplace(get_self(), [&](auto& row) {
        row.package = package;
        row.descriptive_name = descriptive_name;
        row.action_size = action_size;
        row.max_active_series = max_active_series;
        row.max_active_emissions = max_active_emissions;
        row.expiry_duration_in_secs = expiry_duration_in_secs;
        row.cost = cost;
        row.active = active;
    });
}

ACTION subscription::disablepack(name package) {
    require_auth(get_self()); // Ensure only the contract owner can disable a package

    // Open the packages table
    packages_table packages(get_self(), get_self().value);

    // Find the package by its primary key
    auto package_itr = packages.find(package.value);
    check(package_itr != packages.end(), "Package not found.");

    // Modify the record to set active to false
    packages.modify(package_itr, get_self(), [&](auto& row) {
        row.active = false;
    });
}

ACTION subscription::enablepack(name package) {
    require_auth(get_self()); // Ensure only the contract owner can enable a package

    // Open the packages table
    packages_table packages(get_self(), get_self().value);

    // Find the package by its primary key
    auto package_itr = packages.find(package.value);
    check(package_itr != packages.end(), "Package not found.");

    // Modify the record to set active to true
    packages.modify(package_itr, get_self(), [&](auto& row) {
        row.active = true;
    });
}

ACTION subscription::setsercost(extended_asset additional_series_cost) {
    require_auth(get_self()); // Ensure action is authorized by the contract account

    // Validate input
    check(additional_series_cost.quantity.amount >= 0, "Cost cannot be negative");
    check(additional_series_cost.quantity.is_valid(), "Invalid asset");
    check(is_account(additional_series_cost.contract), "Invalid contract account");

    // Access the table
    seriescost_table _seriescost(get_self(), get_self().value);

    // Find the record by the symbol code
    auto itr = _seriescost.find(additional_series_cost.quantity.symbol.code().raw());

    if (itr == _seriescost.end()) {
        // If record does not exist, create a new one
        _seriescost.emplace(get_self(), [&](auto& row) {
            row.additional_series_cost = additional_series_cost;
        });
    } else {
        // If record exists, modify it
        _seriescost.modify(itr, get_self(), [&](auto& row) {
            row.additional_series_cost = additional_series_cost;
        });
    }
}

ACTION subscription::setemitcost(extended_asset additional_emission_cost) {
    require_auth(get_self()); // Ensure action is authorized by the contract account

    // Validate input
    check(additional_emission_cost.quantity.amount >= 0, "Emission cost cannot be negative");
    check(additional_emission_cost.quantity.is_valid(), "Invalid asset");
    check(is_account(additional_emission_cost.contract), "Invalid contract account for the asset");

    // Access the emissioncost table
    emissioncost_table _emissioncost(get_self(), get_self().value);

    // Find a record with the same symbol code
    auto itr = _emissioncost.find(additional_emission_cost.quantity.symbol.code().raw());

    if (itr == _emissioncost.end()) {
        // If the record does not exist, create a new one
        _emissioncost.emplace(get_self(), [&](auto& row) {
            row.additional_emission_cost = additional_emission_cost;
        });
    } else {
        // If the record exists, update it
        _emissioncost.modify(itr, get_self(), [&](auto& row) {
            row.additional_emission_cost = additional_emission_cost;
        });
    }
}

