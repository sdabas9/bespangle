#include <subscription.hpp>

ACTION subscription::cleanupram() {
    ramcost_table ramcosts(get_self(), get_self().value);

    // Find the first record by accessing the table's iterator to the beginning
    auto itr = ramcosts.begin();
    check(itr != ramcosts.end(), "No records available to delete.");

    // Erase the first record
    ramcosts.erase(itr);

    print("First record deleted successfully.");
}

void subscription::startseries(name org, symbol agg_symbol, vector<uint64_t> seq_ids) {
        
        uint64_t free_series = get_perk_value(name("freeseries"));
        uint64_t free_cycles = get_perk_value(name("freesercycle"));

        series_table series(get_self(), get_self().value);
        auto series_itr = series.find(org.value);
        if(series_itr == series.end()) {
            series.emplace(get_self(), [&](auto& s) {
                s.org = org;
                s.free_series = free_series;
                s.free_cycles = free_cycles;
                s.current_active_series = seq_ids.size();
                s.highest_no_of_active_series_in_current_cycle = seq_ids.size();
                s.service_active = true;
            });
        } else {
            series.modify(series_itr, get_self(), [&](auto& s) {
                // Append or modify the unique_issuance_hll data
                uint8_t new_active_series = s.current_active_series + seq_ids.size();
                s.current_active_series = new_active_series;
                if(new_active_series > s.highest_no_of_active_series_in_current_cycle) {
                    s.highest_no_of_active_series_in_current_cycle = new_active_series;
                }
            });
        }

}

void subscription::endseries(name org, symbol agg_symbol, vector<uint64_t> seq_ids) {
        
        uint64_t free_series = get_perk_value(name("freeseries"));
        uint64_t free_cycles = get_perk_value(name("freesercycle"));

        series_table series(get_self(), get_self().value);
        auto series_itr = series.find(org.value);
        if(series_itr == series.end()) {
            series.emplace(get_self(), [&](auto& s) {
                s.org = org;
                s.free_series = free_series;
                s.free_cycles = free_cycles;
                s.current_active_series = 0;
                s.highest_no_of_active_series_in_current_cycle = 0;
                s.service_active = true;
            });
        } else {
            series.modify(series_itr, get_self(), [&](auto& s) {
                // Append or modify the unique_issuance_hll data
                uint8_t new_active_series = s.current_active_series - seq_ids.size();
                if(new_active_series > 0) {
                    s.current_active_series = new_active_series;
                } else {
                    s.current_active_series = 0;
                }
            });
        }
}

ACTION subscription::cleanup(name org, uint64_t new_seq_id, uint64_t curr_seq_id) {
                newpackage_table newpackage(get_self(), org.value);

            // Ensure the newpackage table is not empty
            auto newpackage_itr = newpackage.begin();
                        newpackage.erase(newpackage_itr);

                                currpackage_table currpackage(get_self(), org.value);
        auto currpackage_itr = currpackage.begin();
        currpackage.erase(currpackage_itr);
}

TABLE package {}

void subscription::notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts) {

    // expiration
    // no of issuance
    // no of series 

// no of members
// no of issuance
// no of unique issuance

}

void subscription::notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts) { 
    check_series_payment_due(org);
    
    members_table members(get_self(), get_self().value);
    auto members_itr = members.find(org.value);
   
    if(members_itr == members.end()) {
        init_membership_usage(members, org, to);
        init_series_usage(org);
    } else {
        check(members_itr->service_active == true, "Check if org need to pay member due amount");
        uint8_t b = 7;
        uint32_t m = 1 << b;
        vector<uint8_t> M = members_itr->member_count_hll;
        hll::HyperLogLog hll(b, m, M);
        double estimate_before = hll.estimate();
        hll.add(to.to_string().c_str(), to.to_string().size());
        double estimate_after = hll.estimate();
        if (estimate_before != estimate_after) {
            members.modify(members_itr, get_self(), [&](auto& row) {
                // Append or modify the unique_issuance_hll data
                row.member_count_hll = hll.registers();
                row.member_count += 1;
            });
        }   
    }

    update_package_usage(org, badge_asset, to); 


}

void subscription::buyservice(name from, name to, asset amount, string memo) {
    // Ensure the contract is the recipient of the transfer
    if (to != get_self()) return;

    // Parse the memo string of the format "<org>:PACK:<package>"
    auto memo_parts = split_pack_memo(memo);
    check(memo_parts.size() == 2 || memo_parts.size() == 3, "Memo format is incorrect. Expected <org>:pack:<package> OR <org>:membership OR <org>:series");
    name org = name(memo_parts[0]);
    if(memo_parts[1] == "membership") {
        update_member_due_amount(org, amount, from);
    } else if (memo_parts[1] == "series") {
        update_series_due_amount(org, amount, from);
    } else if (memo_parts[1] == "pack") {
        name package_name = name(memo_parts[2]);
        buy_new_package (org, package_name, amount, from);
    } else {
        check (false, "Memo format is incorrect. Expected <org>:pack:<package> OR <org>:membership OR <org>:series");
    }
    



}

void subscription::buypackwram(name from, name to, asset amount, string memo) {
    // Ensure the contract is not the sender (avoids self transfers)
    if (from == get_self() || to != get_self()) return;

    // Split memo string "<ORG>:PACK:<package>" to extract organization and package
    auto memo_parts = split_pack_memo(memo);
    check(memo_parts.size() == 3, "Memo format is incorrect. Expected <ORG>:PACK:<package>");

    name org = name(memo_parts[0]);    // Extract the organization (ORG)
    name package_name = name(memo_parts[2]); // Extract the package name

    // Lookup the package in the packages table
    packages_table packages(get_self(), get_self().value);
    auto package_itr = packages.find(package_name.value);
    check(package_itr != packages.end(), "Package not found");
    check(package_itr->active, "Package is not active");

    asset package_cost = package_itr->ram_in_bytes;
    check(amount >= package_cost, "Insufficient funds for purchasing the package");

     // Get or create the sequence entry for package in the sequences table
    sequences_table sequences(get_self(), get_self().value);
    auto seq_itr = sequences.find("package"_n.value);

    uint64_t seq_id = get_next_sequence(name("newpackage"));

    // Insert the record into newpackage table scoped by the organization
    newpackage_table newpackage(get_self(), org.value);

    newpackage.emplace(get_self(), [&](auto& row) {
        row.seq_id = seq_id;
        row.package_name = package_name;
        row.unique_issuance_size = package_itr->unique_issuance_size;
        row.expiry_duration_in_secs =  package_itr->expiry_duration_in_secs;
    });

    // Calculate how much balance to return
    asset balance_to_return = amount - package_cost;
    check(balance_to_return.amount >= 0, "Insufficient funds for purchasing the package");

    // Return the balance to the `from` account
    if (balance_to_return.amount > 0) {
        action(
            permission_level{get_self(), "active"_n},
            "eosio.wram"_n, "transfer"_n,
            std::make_tuple(get_self(), from, balance_to_return, std::string("Returning unused balance"))
        ).send();
    }
}

ACTION subscription::newpack(
    name package,
    uint64_t unique_issuance_size,
    uint64_t expiry_duration_in_secs,
    asset ram_in_bytes,
    bool only_ram_as_payment_method,
    bool active) {
    // Ensure the action is authorized by the `authorized` account
    require_auth(get_self());

    // Instantiate the packages table
    packages_table packages(get_self(), get_self().value);

    // Check if the package already exists
    auto package_itr = packages.find(package.value);
    check(package_itr == packages.end(), "Package with this name already exists");

    // Add a new entry in the packages table
    packages.emplace(get_self(), [&](auto& row) {
        row.package = package;
        row.unique_issuance_size = unique_issuance_size;
        row.expiry_duration_in_secs = expiry_duration_in_secs;
        row.ram_in_bytes = ram_in_bytes;
        row.only_ram_as_payment_method = only_ram_as_payment_method;
        row.active = active;
    });
}

ACTION subscription::disablepack(name package) {
    // Ensure the action is authorized by the `authorized` account
    require_auth(get_self());

    // Instantiate the packages table
    packages_table packages(get_self(), get_self().value);

    // Find the package by its name
    auto package_itr = packages.find(package.value);
    check(package_itr != packages.end(), "Package not found");

    // Modify the package to set `active` to false
    packages.modify(package_itr, get_self(), [&](auto& row) {
        row.active = false;
    });
}

ACTION subscription::enablepack(name package) {
    // Ensure the action is authorized by the contract account
    require_auth(get_self());

    // Instantiate the packages table
    packages_table packages(get_self(), get_self().value);

    // Find the package by its name
    auto package_itr = packages.find(package.value);
    check(package_itr != packages.end(), "Package not found");

    // Modify the package to set `active` to true
    packages.modify(package_itr, get_self(), [&](auto& row) {
        row.active = true;
    });
}

ACTION subscription::setvt(uint64_t vt_percent) {
    // Ensure the action is authorized by the contract itself
    require_auth(get_self());

    // Get or create the sequence entry for volatility in the sequences table
    sequences_table sequences(get_self(), get_self().value);
    auto seq_itr = sequences.find("volatility"_n.value);

    uint64_t seq_id;

    if (seq_itr == sequences.end()) {
        // If the sequence does not exist, create it with a starting value
        sequences.emplace(get_self(), [&](auto& row) {
            row.key = "volatility"_n;
            row.last_used_value = 1;
            seq_id = 1;  // Start the seq_id from 1
        });
    } else {
        // If the sequence exists, increment the last_used_value
        seq_id = seq_itr->last_used_value + 1;
        sequences.modify(seq_itr, get_self(), [&](auto& row) {
            row.last_used_value = seq_id;
        });
    }

    // Instantiate the volatility table
    volatility_table volatility(get_self(), get_self().value);

    // Insert a new record into the volatility table
    volatility.emplace(get_self(), [&](auto& row) {
        row.seq_id = seq_id;
        row.value = vt_percent;  // Store as a percentage with 2 decimals (e.g., 2534 for 25.34%)
        row.last_updated = current_time_point();  // Set the current time as last_updated
    });
}

ACTION subscription::setramrate(extended_asset per_byte_cost) {
    // Ensure the action is authorized by the contract itself
    require_auth(get_self());

    // Get or create the sequence entry for ramcost in the sequences table
    sequences_table sequences(get_self(), get_self().value);
    auto seq_itr = sequences.find("ramcost"_n.value);

    uint64_t seq_id;

    if (seq_itr == sequences.end()) {
        // If the sequence does not exist, create it with a starting value
        sequences.emplace(get_self(), [&](auto& row) {
            row.key = "ramcost"_n;
            row.last_used_value = 1;
            seq_id = 1;  // Start the seq_id from 1
        });
    } else {
        // If the sequence exists, increment the last_used_value
        seq_id = seq_itr->last_used_value + 1;
        sequences.modify(seq_itr, get_self(), [&](auto& row) {
            row.last_used_value = seq_id;
        });
    }

    // Instantiate the ramcost table
    ramcost_table ramcost(get_self(), get_self().value);

    // Insert a new record into the ramcost table
    ramcost.emplace(get_self(), [&](auto& row) {
        row.seq_id = seq_id;
        row.per_byte_cost = per_byte_cost;  // Store the fixed cost per byte
        row.last_updated = current_time_point();  // Set the current time as last_updated
    });
}

ACTION subscription::firstcycle(time_point_sec start_time, time_point_sec end_time) {
    require_auth(get_self());  // Only the contract account can call this action

    // Ensure the start time is before the end time
    check(start_time < end_time, "Start time must be before end time.");

    // Check if the billcycle table already contains a record
    billcycle_table bill_cycles(get_self(), get_self().value);
    auto bill_cycle_itr = bill_cycles.begin();
    check(bill_cycle_itr == bill_cycles.end(), "A billing cycle record already exists.");

    // Initialize the sequences table to track the sequence for bill cycles
    sequences_table sequences(get_self(), get_self().value);

    // Look for the sequence record for 'billcycle'
    auto seq_itr = sequences.find("billcycle"_n.value);

    uint64_t seq_id;

    if (seq_itr == sequences.end()) {
        // If no sequence exists, initialize it
        seq_id = 1;  // Start the sequence at 1
        sequences.emplace(get_self(), [&](auto& seq) {
            seq.key = "billcycle"_n;
            seq.last_used_value = seq_id;
        });
    } else {
        // If a sequence exists, increment it
        seq_id = seq_itr->last_used_value + 1;
        sequences.modify(seq_itr, get_self(), [&](auto& seq) {
            seq.last_used_value = seq_id;
        });
    }

    // Add a new entry to the billcycle table
    bill_cycles.emplace(get_self(), [&](auto& new_cycle) {
        new_cycle.seq_id = seq_id;
        new_cycle.start_time = start_time;
        new_cycle.end_time = end_time;
    });

    // Print success message
    print("First billing cycle added with sequence ID: ", seq_id);
}

ACTION subscription::nextcycle(time_point_sec end_time) {
    require_auth(get_self());  // Ensure only the contract can execute this action

    // Get the next sequence ID for billcycle
    uint64_t next_seq_id = get_last_sequence("billcycle"_n) + 1;

    // Load the billcycle table and find the last entered cycle by sequence ID
    billcycle_table bill_cycles(get_self(), get_self().value);
    auto last_cycle_itr = bill_cycles.find(next_seq_id - 1);  // Get the previous cycle

    // Ensure the previous cycle exists
    check(last_cycle_itr != bill_cycles.end(), "Previous cycle not found. Cannot create next cycle.");

    // Calculate the start time for the new cycle as the end time of the last cycle + 1 second
    time_point_sec start_time = time_point_sec(last_cycle_itr->end_time.sec_since_epoch() + 1);

    // Ensure the provided end_time is greater than the calculated start_time
    check(end_time > start_time, "End time must be greater than the start time.");

    // Update the sequences table with the new sequence value
    sequences_table seq_tbl(get_self(), get_self().value);
    auto seq_itr = seq_tbl.find("billcycle"_n.value);

    if (seq_itr == seq_tbl.end()) {
        seq_tbl.emplace(get_self(), [&](auto& seq) {
            seq.key = "billcycle"_n;
            seq.last_used_value = next_seq_id;
        });
    } else {
        seq_tbl.modify(seq_itr, get_self(), [&](auto& seq) {
            seq.last_used_value = next_seq_id;
        });
    }

    // Add the new cycle to the billcycle table
    bill_cycles.emplace(get_self(), [&](auto& new_cycle) {
        new_cycle.seq_id = next_seq_id;
        new_cycle.start_time = start_time;
        new_cycle.end_time = end_time;
    });

}

ACTION subscription::setduetime(uint64_t due_grace_seconds) {
    require_auth(get_self());  // Ensure only the contract can execute this action

    // Load the duetime singleton
    duetime_singleton duetime_tbl(get_self(), get_self().value);

    // Set or update the due_grace_seconds in the singleton table
    duetime_tbl.set(duetime{
        .due_grace_seconds = due_grace_seconds
    }, get_self());

    // Print success message
    print("Due time updated: due_grace_seconds = ", due_grace_seconds);
}

ACTION subscription::memberbill(name org) {
    require_auth(get_self());  // Ensure only the contract can execute this action

    // Load the current block time
    time_point_sec current_time = eosio::current_time_point();

    // Find the current cycle in the billcycle table
    billcycle_table bill_cycles(get_self(), get_self().value);
    auto current_cycle_itr = bill_cycles.begin();

    // Move past outdated cycles to oldbillcycle
    while (current_cycle_itr != bill_cycles.end() && current_time > current_cycle_itr->end_time) {
        move_to_oldbillcycle(*current_cycle_itr);
        current_cycle_itr = bill_cycles.begin();
    }

    // Ensure there is a current cycle
    check(current_cycle_itr != bill_cycles.end(), "No current billing cycle available.");

    // Load the members table and find the organization
    members_table members(get_self(), get_self().value);
    auto member_itr = members.find(org.value);
    check(member_itr != members.end(), "Organization not found in members table.");

    // Ensure the action runs only once per cycle
    check(member_itr->last_billed_amount_calculation_time < current_cycle_itr->start_time, 
          "Bill has already been generated for this cycle.");

    // If the subscription started in the current cycle, the due amount is 0
    if (member_itr->free_cycles > 0 || 
        member_itr->free_members >= member_itr->member_count) {
        members.modify(member_itr, get_self(), [&](auto& member) {
            member.last_billed_amount_calculation_time = current_time;
            member.free_cycles -= 1;
        });
        print("No bill generated, first subscription in current cycle.");
        return;
    }

    // Load the admincost table to get the first member admin cost
    membercost_table member_costs(get_self(), get_self().value);
    auto member_cost_itr = member_costs.begin();
    check(member_cost_itr != member_costs.end(), "Admin cost not defined in the admincost table.");

    // Calculate the due amount using extended_asset
    uint64_t additional_members = member_itr->member_count - member_itr->free_members;
    extended_asset total_due = member_cost_itr->first_additional_member_admin_cost;
    total_due.quantity.amount *= additional_members;

    // Modify the member record with the calculated due amount
    members.modify(member_itr, get_self(), [&](auto& member) {
        // Check if the total_due_amount is uninitialized or set for the first time
        if (member.total_due_amount.quantity.amount == 0 && 
            !member.total_due_amount.quantity.symbol.is_valid()) {
            // Initialize the total_due_amount for the first time
            member.total_due_amount = total_due;
        } else {
            // Ensure symbol consistency
            check(member.total_due_amount.contract == total_due.contract, "contract mismatch while updating due amount");;

            check(member.total_due_amount.quantity.symbol == total_due.quantity.symbol, 
                  "Symbol mismatch while updating total due amount");
            // Add to the existing total due amount
            member.total_due_amount.quantity += total_due.quantity;
        }

        // Update the last billed time
        member.last_billed_amount_calculation_time = current_time;
    });

    // Print success message
    print("Admin bill generated successfully for org: ", org, 
          ", Total Due: ", total_due.quantity, 
          " with contract: ", total_due.contract);
}


ACTION subscription::chkmemstatus(name org) {
    require_auth(get_self());  // Ensure the contract itself is executing the action

    // Load the current block time
    time_point_sec current_time = eosio::current_time_point();

    // Load the billcycle table to find the current cycle
    billcycle_table bill_cycles(get_self(), get_self().value);
    auto current_cycle_itr = bill_cycles.begin();

    // Move past outdated cycles to oldbillcycle
    while (current_cycle_itr != bill_cycles.end() && current_time > current_cycle_itr->end_time) {
        move_to_oldbillcycle(*current_cycle_itr);
        current_cycle_itr = bill_cycles.begin();
    }

    // Ensure there is a current cycle
    check(current_cycle_itr != bill_cycles.end(), "No current billing cycle available.");

    // Load the mduetime singleton to get the due grace period
    duetime_singleton due_time(get_self(), get_self().value);
    check(due_time.exists(), "Due grace time is not set.");
    auto due_grace = due_time.get();

    // Calculate due time as the current cycle start time + due_grace_seconds
    time_point_sec due_time_point = time_point_sec(current_cycle_itr->start_time.sec_since_epoch() + due_grace.due_grace_seconds);

    // Load the members table to get the member details
    members_table members(get_self(), get_self().value);
    auto member_itr = members.find(org.value);
    check(member_itr != members.end(), "Organization not found in members table.");

    // Determine the service status
    bool service_active = true;
    if (current_time > due_time_point && member_itr->total_due_amount.quantity.amount > 0) {
        service_active = false;  // Past due time and unpaid amount exists
    }

    // Update the member's service_active status
    members.modify(member_itr, get_self(), [&](auto& member) {
        member.service_active = service_active;
    });

    // Print the result
    print("Service status for org: ", org, " updated to: ", service_active ? "active" : "inactive");
}

ACTION subscription::setmemstatus(name org, bool service_active) {
    require_auth(get_self());  // Ensure the contract itself is authorized to execute this action

    // Load the members table
    members_table members(get_self(), get_self().value);

    // Find the member record for the given organization
    auto member_itr = members.find(org.value);
    check(member_itr != members.end(), "Organization not found in members table.");

    // Update the service_active status
    members.modify(member_itr, get_self(), [&](auto& member) {
        member.service_active = service_active;
    });

    // Print success message
    print("Service status for org: ", org, " updated to: ", service_active ? "active" : "inactive");
}

ACTION subscription::setmemcost(extended_asset first_additional_member_admin_cost) {
    require_auth(get_self());  // Ensure only the contract can execute this action

    // Validate that the asset is non-negative
    check(first_additional_member_admin_cost.quantity.amount >= 0, "Admin cost must be non-negative.");

    // Load the membercost table
    membercost_table member_costs(get_self(), get_self().value);

    // Check if the cost already exists for this symbol
    auto cost_itr = member_costs.find(first_additional_member_admin_cost.quantity.symbol.code().raw());

    if (cost_itr == member_costs.end()) {
        // If not found, add a new entry
        member_costs.emplace(get_self(), [&](auto& cost) {
            cost.first_additional_member_admin_cost = first_additional_member_admin_cost;
        });
    } else {
        // If found, update the existing entry
        member_costs.modify(cost_itr, get_self(), [&](auto& cost) {
            cost.first_additional_member_admin_cost = first_additional_member_admin_cost;
        });
    }

    // Print success message
    print("Member admin cost set to: ", first_additional_member_admin_cost.quantity, " with contract: ", first_additional_member_admin_cost.contract);
}


ACTION subscription::seriesbill(name org) {
    require_auth(get_self());  // Ensure the contract itself is authorized to execute this action

    // Load the current block time
    time_point_sec current_time = eosio::current_time_point();

    // Load the billcycle table to find the current cycle
    billcycle_table bill_cycles(get_self(), get_self().value);
    auto current_cycle_itr = bill_cycles.begin();

    // Move past outdated cycles to oldbillcycle
    while (current_cycle_itr != bill_cycles.end() && current_time > current_cycle_itr->end_time) {
        move_to_oldbillcycle(*current_cycle_itr);
        current_cycle_itr = bill_cycles.begin();
    }

    // Ensure there is a current cycle
    check(current_cycle_itr != bill_cycles.end(), "No current billing cycle available.");

    // Load the series table to get the member's series details
    series_table series(get_self(), get_self().value);
    auto series_itr = series.find(org.value);
    check(series_itr != series.end(), "Organization not found in series table.");

    // Ensure the action is only executed once per cycle
    check(series_itr->last_billed_amount_calculation_time < current_cycle_itr->start_time, 
          "Bill has already been generated for this cycle.");

    // If the highest number of active series is within the free limit, the due amount is 0
    if (series_itr->free_cycles > 0 || 
        series_itr->free_series >= series_itr->highest_no_of_active_series_in_current_cycle) {
        series.modify(series_itr, get_self(), [&](auto& s) {
            s.last_billed_amount_calculation_time = current_time;
            s.free_cycles -= 1;
            s.highest_no_of_active_series_in_current_cycle = s.current_active_series;
        });        
        print("No bill generated, active series are within the free limit.");
        return;
    }

    // Load the seriescost table to get the additional series admin cost
    seriescost_table series_costs(get_self(), get_self().value);
    auto series_cost_itr = series_costs.begin();
    check(series_cost_itr != series_costs.end(), "Series admin cost not defined in the seriescost table.");

    // Calculate the due amount using extended_asset
    uint8_t additional_series = series_itr->highest_no_of_active_series_in_current_cycle - series_itr->free_series;
    extended_asset total_due = series_cost_itr->additonal_series_admin_cost;
    total_due.quantity.amount *= additional_series;

    // Update the series with the total due amount and the current time
    series.modify(series_itr, get_self(), [&](auto& series) {
        // Check if the total_due_amount is uninitialized or set for the first time
        if (series.total_due_amount.quantity.amount == 0 && 
            !series.total_due_amount.quantity.symbol.is_valid()) {
            // Initialize the total_due_amount for the first time
            series.total_due_amount = total_due;
        } else {
            // Ensure symbol consistency
            check(series.total_due_amount.contract == total_due.contract, "contract mismatch while updating due amount");;
            check(series.total_due_amount.quantity.symbol == total_due.quantity.symbol, 
                  "Symbol mismatch while updating total due amount");
            // Add to the existing total due amount
            series.total_due_amount += total_due;
        }
        series.highest_no_of_active_series_in_current_cycle = series.current_active_series;
        // Update the last billed time
        series.last_billed_amount_calculation_time = current_time;
    });

    // Print success message
    print("Series bill generated successfully for org: ", org, 
          ", Total Due: ", total_due.quantity, 
          " with contract: ", total_due.contract);
}


ACTION subscription::chkserstatus(name org) {
    require_auth(get_self());  // Ensure only the contract can execute this action

    // Load the current block time
    time_point_sec current_time = eosio::current_time_point();

    // Find the current cycle in the billcycle table
    billcycle_table bill_cycles(get_self(), get_self().value);
    auto current_cycle_itr = bill_cycles.begin();

    // Move past outdated cycles to oldbillcycle
    while (current_cycle_itr != bill_cycles.end() && current_time > current_cycle_itr->end_time) {
        move_to_oldbillcycle(*current_cycle_itr);
        current_cycle_itr = bill_cycles.begin();
    }

    // Ensure there is a current cycle
    check(current_cycle_itr != bill_cycles.end(), "No current billing cycle available.");

    // Load the due time from the duetime singleton
    duetime_singleton due_time(get_self(), get_self().value);
    check(due_time.exists(), "Due time is not set.");
    auto grace_period = due_time.get();

    // Calculate the due time as start time + due_grace_seconds
    time_point_sec due_time_point = time_point_sec(
        current_cycle_itr->start_time.sec_since_epoch() + grace_period.due_grace_seconds);

    // Load the series table and find the organization
    series_table series(get_self(), get_self().value);
    auto series_itr = series.find(org.value);
    check(series_itr != series.end(), "Organization not found in series table.");

    // Determine the service status based on due time and total due amount
    bool service_active = true;
    if (current_time > due_time_point && series_itr->total_due_amount.quantity.amount > 0) {
        service_active = false;
    }

    // Update the service status in the series table
    series.modify(series_itr, get_self(), [&](auto& s) {
        s.service_active = service_active;
    });

    // Print success message
    print("Service status for org: ", org, " updated to: ", service_active ? "active" : "inactive");
}

ACTION subscription::setserstatus(name org, bool service_active) {
    require_auth(get_self());  // Ensure only the contract can execute this action

    // Load the series table and find the organization
    series_table series(get_self(), get_self().value);
    auto series_itr = series.find(org.value);
    check(series_itr != series.end(), "Organization not found in series table.");

    // Update the service_active status
    series.modify(series_itr, get_self(), [&](auto& s) {
        s.service_active = service_active;
    });

    // Print success message
    print("Service status for org: ", org, " updated to: ", service_active ? "active" : "inactive");
}

ACTION subscription::setsercost(extended_asset additonal_series_admin_cost) {
    require_auth(get_self());  // Ensure only the contract can execute this action

    // Validate that the asset is non-negative
    check(additonal_series_admin_cost.quantity.amount >= 0, "Admin cost must be non-negative.");

    // Load the seriescost table
    seriescost_table series_costs(get_self(), get_self().value);

    // Check if the cost already exists for this symbol
    auto cost_itr = series_costs.find(additonal_series_admin_cost.quantity.symbol.code().raw());

    if (cost_itr == series_costs.end()) {
        // If not found, add a new entry
        series_costs.emplace(get_self(), [&](auto& cost) {
            cost.additonal_series_admin_cost = additonal_series_admin_cost;
        });
    } else {
        // If found, update the existing entry
        series_costs.modify(cost_itr, get_self(), [&](auto& cost) {
            cost.additonal_series_admin_cost = additonal_series_admin_cost;
        });
    }

    // Print success message
    print("Series admin cost set to: ", additonal_series_admin_cost.quantity, " with contract: ", additonal_series_admin_cost.contract);
}


// ACTION to insert/update free_members
ACTION subscription::freemembers(uint64_t free_members) {
    require_auth(get_self()); 
    insert_or_update_perk("freemembers"_n, free_members);
}

// ACTION to insert/update free_cycles for free membership cycles
ACTION subscription::freememcycle(uint64_t free_cycles) {
    require_auth(get_self()); 
    insert_or_update_perk("freememcycle"_n, free_cycles);
}

// ACTION to insert/update free_series
ACTION subscription::freeseries(uint64_t free_series) {
    require_auth(get_self()); 
    insert_or_update_perk("freeseries"_n, free_series);
}

// ACTION to insert/update free_cycles for free series cycles
ACTION subscription::freesercycle(uint64_t free_cycles) {
    require_auth(get_self()); 
    insert_or_update_perk("freesercycle"_n, free_cycles);
}



