#include <billing.hpp>


    void receive(name from, name to, asset quantity, string memo) {
        // Ignore transfers that are not to this contract
        if (to != get_self()) {
            return;
        }

        // Ensure transfer is not from the contract itself or system accounts
        check(from != get_self(), "Contract cannot send transfers to itself.");
        check(from != "eosio"_n, "System account cannot transfer.");

        // Ensure that the asset is in the correct symbol
        check(quantity.symbol == symbol("EOS", 4), "Only EOS tokens are accepted.");

        // Parse the memo to extract the pack seq_id
        uint64_t pack_seq_id = std::stoull(memo);

        // Access the packs table to get the corresponding pack
        packs_table packs(get_self(), get_self().value);
        auto pack_itr = packs.find(pack_seq_id);
        check(pack_itr != packs.end(), "Pack with the given seq_id not found.");

        // Check if transferred amount is greater than or equal to the pack cost
        check(quantity.amount >= pack_itr->cost.amount, "Transferred amount is less than the required cost for the selected pack.");

        // If excess balance, refund the excess to the sender
        if (quantity.amount > pack_itr->cost.amount) {
            asset excess = quantity - pack_itr->cost;

            // Send excess back to the sender
            action(
                permission_level{get_self(), "active"_n},
                "eosio.token"_n,
                "transfer"_n,
                std::make_tuple(get_self(), from, excess, std::string("Refunding excess balance"))
            ).send();
        }

        // Access the orgpacks table
        orgpacks_table orgpacks(get_self(), get_self().value);
        auto status_index = orgpacks.get_index<"bystatus"_n>();
        auto expiry_index = orgpacks.get_index<"byexpiry"_n>();

        // Step 1: Check if there is any active record and if it's expired
        auto active_itr = status_index.find("active"_n.value);
        bool found_active = false;

        if (active_itr != status_index.end()) {
            // If the active pack has expired, set it to 'expired'
            if (active_itr->expiry_time <= time_point(current_time_point())) {
                status_index.modify(active_itr, get_self(), [&](auto& row) {
                    row.status = "expired"_n;
                });
                print("Record with seq_id ", active_itr->seq_id, " was set to expired.");
            } else {
                found_active = true;
                print("Active record with seq_id ", active_itr->seq_id, " remains active.");
            }
        }

        // Step 2: Find the earliest available pack if no active pack was found or the previous active expired
        if (!found_active) {
            auto available_itr = expiry_index.lower_bound(time_point_sec(current_time_point()).elapsed.count());
            for (; available_itr != expiry_index.end() && available_itr->status == "available"_n; ++available_itr) {
                // Set the earliest available pack to 'active'
                expiry_index.modify(available_itr, get_self(), [&](auto& row) {
                    row.status = "active"_n;
                });
                print("Record with seq_id ", available_itr->seq_id, " is now active.");
                found_active = true;
                break;
            }
        }

        // Step 3: Insert a new record as the last one with the furthest expiry_time
        time_point furthest_expiry_time = time_point(current_time_point());  // Default to current time if no available or active packs
        if (expiry_index.begin() != expiry_index.end()) {
            auto furthest_itr = --expiry_index.end();  // Get the record with the furthest expiry_time
            furthest_expiry_time = furthest_itr->expiry_time;
        }

        // Calculate the new expiry_time: furthest_expiry_time + expiry_duration
        time_point_sec new_expiry_time = furthest_expiry_time + seconds(pack_itr->expiry_duration);

        // Insert a new record in the orgpacks table with status based on whether an active pack exists
        orgpacks.emplace(get_self(), [&](auto& row) {
            row.seq_id = orgpacks.available_primary_key();
            row.status = found_active ? "available"_n : "active"_n;  // Active if no other active packs
            row.allowed = pack_itr->number_of_issuances;
            row.used = 0;
            row.expiry_time = new_expiry_time;
        });

        print("New record inserted in orgpacks with seq_id: ", orgpacks.available_primary_key());
    }

EOSIO_DISPATCH(billing, (hi)(clear))
