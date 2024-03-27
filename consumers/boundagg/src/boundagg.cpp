#include <boundagg.hpp>

    ACTION boundagg::addbadgeli(symbol agg_symbol, vector<symbol> badge_symbols) {
        string action_name = "addbadgeli";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);

        name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

        
        // Retrieve the last_init_seq_id for given agg_symbol from agginfo table
        agginfo_table agginfos(get_self(), org.value);
        auto agginfo_itr = agginfos.find(agg_symbol.code().raw());
        check(agginfo_itr != agginfos.end(), "Aggregation information not found");

        uint64_t last_init_seq_id = agginfo_itr->last_init_seq_id;
        
        // Retrieve seq_status from the agg table
        name seq_status = get_seq_status(agg_symbol, last_init_seq_id, failure_identifier);
        
        // Add badges to the badgeaggseq table
        add_badges_to_aggseq(agg_symbol, last_init_seq_id, badge_symbols, seq_status, failure_identifier);
    }

    ACTION boundagg::addbadge(symbol agg_symbol, vector<uint64_t> seq_ids, vector<symbol> badge_symbols) {
        string action_name = "addbadge";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);


        // Iterate through each combination of seq_id and badge_symbol
        for (auto seq_id : seq_ids) {
            // Retrieve seq_status from the agg table for each seq_id
            name seq_status = get_seq_status(agg_symbol, seq_id, failure_identifier);
            add_badges_to_aggseq(agg_symbol, seq_id, badge_symbols, seq_status, failure_identifier);
        }
    }

    ACTION boundagg::initagg(symbol agg_symbol, string description) {
        string action_name = "initagg";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        // Derive the org from the agg_symbol; assuming an implementation detail
        // that maps symbols to organizations, perhaps via a naming convention or auxiliary table.
        name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

        // Check if this agg_symbol already exists in the agginfo table and update or create accordingly
        agginfo_table agginfos(get_self(), org.value);
        auto agginfo_itr = agginfos.find(agg_symbol.code().raw());
        
        uint64_t new_seq_id = 0;
        if(agginfo_itr == agginfos.end()) {
            // agg_symbol does not exist, create new agginfo
            agginfos.emplace(get_self(), [&](auto& info) {
                info.agg_symbol = agg_symbol;
                info.last_init_seq_id = 1; // Starting sequence ID
                new_seq_id = info.last_init_seq_id;
                // Initialize other fields as necessary
                info.last_end_seq_id = 0;
                info.last_active_seq_id = 0;
                info.last_pause_seq_id = 0;
            });
        } else {
            // agg_symbol exists, update last_init_seq_id
            agginfos.modify(agginfo_itr, get_self(), [&](auto& info) {
                info.last_init_seq_id += 1;
                new_seq_id = info.last_init_seq_id;
            });
        }

        // Now, insert the new aggregation sequence into the agg table
        agg_table aggs(get_self(), agg_symbol.code().raw());
        aggs.emplace(get_self(), [&](auto& a) {
            a.seq_id = new_seq_id;
            a.seq_status = "init"_n;
            a.description = description;
            a.init_time = time_point_sec(current_time_point()); // Capture the current time as init time
            // Set start_time, end_time, and pause_time appropriately; likely initialized to zero or a null equivalent
        });
    }

    ACTION boundagg::activeseq(symbol agg_symbol, vector<uint64_t> seq_ids) {
        string action_name = "activeseq";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        for (auto& seq_id : seq_ids) {
            activate_seq_status(agg_symbol, seq_id, failure_identifier);
        }
    }

    ACTION boundagg::activeseqli(symbol agg_symbol) {
        string action_name = "activeseqli";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        uint64_t last_init_seq_id = get_last_init_seq_id(agg_symbol, failure_identifier);
        activate_seq_status(agg_symbol, last_init_seq_id, failure_identifier);
    }

    ACTION boundagg::activeseqlp(symbol agg_symbol) {
        string action_name = "activeseqlp";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        uint64_t last_paused_seq_id = get_last_paused_seq_id(agg_symbol, failure_identifier);
        activate_seq_status(agg_symbol, last_paused_seq_id, failure_identifier);
    }

    void boundagg::notifyachiev(asset badge_asset, name from, name to, string memo, vector<name> notify_accounts) {

        
        badgeaggseq_table badgeaggseqs(get_self(), get_self().value);
        auto by_status_index = badgeaggseqs.get_index<"bybadgestat"_n>();

        // Assuming a method to hash the badge_status and seq_status to "active"
        auto hashed_active_status = hash_active_status(badge_asset.symbol, "active"_n, "active"_n);

        achievements_table achievements(get_self(), to.value);
        for(auto itr = by_status_index.find(hashed_active_status); itr != by_status_index.end() && itr->by_badge_status() == hashed_active_status; ++itr) {
            if(itr->badge_symbol == badge_asset.symbol && itr->badge_status == "active"_n && itr->seq_status == "active"_n) {
                auto ach_itr = achievements.find(itr->badge_agg_seq_id);
                if(ach_itr != achievements.end()) {
                    achievements.modify(ach_itr, get_self(), [&](auto& ach) {
                        ach.count += badge_asset.amount;
                    });
                } else {
                    achievements.emplace(get_self(), [&](auto& ach) {
                        ach.badge_agg_seq_id = itr->badge_agg_seq_id;
                        ach.count = badge_asset.amount;
                    });
                }
            }
        }
    }

    ACTION boundagg::initseq(symbol agg_symbol, string description) {
        string action_name = "initseq";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);

        name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

        // Check if this agg_symbol already exists in the agginfo table and update or create accordingly
        agginfo_table agginfos(get_self(), org.value);
        auto agginfo_itr = agginfos.find(agg_symbol.code().raw());
        check(agginfo_itr != agginfos.end(), "Aggregation symbol not found in agginfo table");

        uint64_t new_seq_id = agginfo_itr->last_init_seq_id + 1;

        // Update the agginfo table with the new last_init_seq_id
        agginfos.modify(agginfo_itr, get_self(), [&](auto& row) {
            row.last_init_seq_id = new_seq_id;
        });

        // Create a new entry in the agg table
        agg_table aggs(get_self(), agg_symbol.code().raw());
        aggs.emplace(get_self(), [&](auto& row) {
            row.seq_id = new_seq_id;
            row.seq_status = "init"_n;
            row.description = description;
            row.init_time = time_point_sec(current_time_point());
            // Initialize other fields as necessary
        });
    }

    ACTION boundagg::pauseseq(symbol agg_symbol, vector<uint64_t> seq_ids) {
        string action_name = "pauseseq";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        // Preconditions: seq_ids is not empty and sorted.
        check(!seq_ids.empty(), "seq_ids should not be empty.");
        uint64_t last_seq_id_in_vector = seq_ids.back();

        // Update agginfo table with the last seq_id in the vector as last_pause_seq_id.
        agginfo_table agginfos(get_self(), agg_symbol.code().raw());
        auto agginfo_itr = agginfos.find(agg_symbol.code().raw());
        check(agginfo_itr != agginfos.end(), "Aggregation info not found for the given symbol.");

        agginfos.modify(agginfo_itr, get_self(), [&](auto& info) {
            info.last_pause_seq_id = last_seq_id_in_vector; // Update last_pause_seq_id
        });

        // Update seq_status in the agg table.
        agg_table aggs(get_self(), agg_symbol.code().raw());
        for (auto& seq_id : seq_ids) {
            auto agg_itr = aggs.find(seq_id);
            if (agg_itr != aggs.end() && agg_itr->seq_status == "active"_n) {
                aggs.modify(agg_itr, get_self(), [&](auto& agg) {
                    agg.seq_status = "pause"_n; // Update to pause
                });
            }
        }

        // Update seq_status in the badgeaggseq table.
        badgeaggseq_table badgeaggseqs(get_self(), get_self().value);
        for (auto& seq_id : seq_ids) {
            auto badge_idx = badgeaggseqs.get_index<"byaggseq"_n>();
            auto lower_bound_key = badgeaggseq::combine_keys(agg_symbol.raw(), seq_id);
            for (auto badge_itr = badge_idx.lower_bound(lower_bound_key);
                 badge_itr != badge_idx.end() && badge_itr->agg_symbol == agg_symbol && badge_itr->seq_id == seq_id;
                 ++badge_itr) {
                if (badge_itr->seq_status == "active"_n) {
                    badgeaggseqs.modify(*badge_itr, get_self(), [&](auto& badge) {
                        badge.seq_status = "pause"_n; // Update to pause
                    });
                }
            }
        }
    }

   ACTION boundagg::endseq(symbol agg_symbol, vector<uint64_t> seq_ids) {
        string action_name = "endseq";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);

        check(!seq_ids.empty(), "Sequence IDs vector must not be empty.");

        // Fetch agginfo record for updating later
        agginfo_table agginfos(get_self(), agg_symbol.code().raw());
        auto agginfo_itr = agginfos.find(agg_symbol.code().raw());
        check(agginfo_itr != agginfos.end(), "Aggregation info not found.");

        // Iterate over the provided seq_ids and update their status in agg and badgeaggseq tables
        for (auto seq_id : seq_ids) {
            // Update seq_status in agg table
            agg_table aggs(get_self(), agg_symbol.code().raw());
            auto agg_itr = aggs.find(seq_id);
            check(agg_itr != aggs.end(), "Sequence ID not found.");
            check(agg_itr->seq_status == "init"_n || agg_itr->seq_status == "active"_n || agg_itr->seq_status == "pause"_n, 
                  "Sequence status must be 'init', 'active', or 'pause' to transition to 'end'.");

            aggs.modify(agg_itr, get_self(), [&](auto& item) {
                item.seq_status = "end"_n;
            });

            // Update seq_status in badgeaggseq table for corresponding seq_id
            badgeaggseq_table badgeaggseqs(get_self(), agg_symbol.code().raw());
            auto badge_idx = badgeaggseqs.get_index<"byaggseq"_n>();
            auto lower_bound_key = badgeaggseq::combine_keys(agg_symbol.raw(), seq_id);
            for (auto badge_itr = badge_idx.lower_bound(lower_bound_key);
                 badge_itr != badge_idx.end() && badge_itr->agg_symbol == agg_symbol && badge_itr->seq_id == seq_id;
                 ++badge_itr) {
                 badgeaggseqs.modify(*badge_itr, get_self(), [&](auto& badge) {
                        badge.seq_status = "end"_n;
                });
            }
        }

        // Update the last_end_seq_id in the agginfo table with the last element in the seq_ids vector
        agginfos.modify(agginfo_itr, get_self(), [&](auto& info) {
            info.last_end_seq_id = seq_ids.back();
        });
    }
 
