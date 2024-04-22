#include <boundagg.hpp>

ACTION boundagg::initagg(name org, symbol agg_symbol, string agg_description) {
    string action_name = "initagg";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    // Access the aggdetail table
    aggdetail_table aggdetail_t(_self, org.value);
    auto aggdetail_itr = aggdetail_t.find(agg_symbol.code().raw());
    if(aggdetail_itr == aggdetail_t.end()) {
        aggdetail_t.emplace(_self, [&](auto& row) {
            row.agg_symbol = agg_symbol;
            row.agg_description = agg_description;
            row.last_init_seq_id = 0;
        });
    } else {
        check(false, "Agg already exists");
    }

}

ACTION boundagg::initseq(name org, symbol agg_symbol, string sequence_description) {
    string action_name = "initseq";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    // Access the aggdetail table
    aggdetail_table aggdetail_t(_self, org.value);
    auto aggdetail_itr = aggdetail_t.find(agg_symbol.code().raw());

    check(aggdetail_itr != aggdetail_t.end(), failure_identifier + "Aggregation symbol does not exist.");

    // Update the last_init_seq_id in aggdetail table
    aggdetail_t.modify(aggdetail_itr, _self, [&](auto& row) {
        row.last_init_seq_id += 1;
        row.init_seq_ids.push_back(row.last_init_seq_id);
    });

    // Insert new entry in the sequence table
    sequence_table sequence_t(get_self(), agg_symbol.code().raw()); // Scope to agg_symbol.code().raw()
    sequence_t.emplace(_self, [&](auto& row) {
        row.seq_id = aggdetail_itr->last_init_seq_id;
        row.seq_status = "init"_n;
        row.sequence_description = sequence_description;
        row.init_time = time_point_sec(current_time_point());
        // The active_time and end_time are left uninitialized here and will be set later
    });
}

ACTION boundagg::actseq(name org, symbol agg_symbol, vector<uint64_t> seq_ids) {
    string action_name = "actseq";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    if(!has_auth(get_self())) {
        check_internal_auth(name(action_name), failure_identifier);
    }

    // Access the aggdetail table
    aggdetail_table aggdetail_t(_self, org.value);
    auto aggdetail_itr = aggdetail_t.find(agg_symbol.code().raw());

    check(aggdetail_itr != aggdetail_t.end(), failure_identifier + "Aggregation symbol does not exist.");

    // Update the last_init_seq_id in aggdetail table
    aggdetail_t.modify(aggdetail_itr, _self, [&](auto& row) {
        vector<uint64_t> init_seq_ids = row.init_seq_ids;
        vector<uint64_t> new_init_seq_ids;
        vector<uint64_t> new_active_seq_ids = row.active_seq_ids;
        for (uint64_t seq_id : init_seq_ids) {
            if (std::find(seq_ids.begin(), seq_ids.end(), seq_id) == seq_ids.end()) {
                new_init_seq_ids.push_back(seq_id);
            } else {
                new_active_seq_ids.push_back(seq_id);
            }
        }
        row.init_seq_ids = new_init_seq_ids;
        row.active_seq_ids = new_active_seq_ids;
    });
    
    sequence_table sequence_t(get_self(), agg_symbol.code().raw());
    for (auto& seq_id : seq_ids) {
        auto itr = sequence_t.find(seq_id);
        check(itr != sequence_t.end(), failure_identifier + "Sequence ID does not exist.");
        check(itr->seq_status == "init"_n, failure_identifier + "Sequence is not in init state.");

        sequence_t.modify(itr, _self, [&](auto& row) {
            row.seq_status = "active"_n;
            row.active_time = time_point_sec(current_time_point());
        });
        update_status_in_badgestatus(org, agg_symbol, seq_id, "active"_n, failure_identifier);
    }

}

ACTION boundagg::actseqai(name org, symbol agg_symbol) {
    string action_name = "actseqai";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    vector<uint64_t> init_seq_ids = all_init_seq_ids (org, agg_symbol, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("actseq"),
      actseq_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = init_seq_ids
      }
    }.send();

}

ACTION boundagg::actseqfi(name org, symbol agg_symbol) {
    string action_name = "actseqfi";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> init_seq_ids;
    uint64_t first_seq_id = all_init_seq_ids (org, agg_symbol, failure_identifier).front();
    init_seq_ids.push_back(first_seq_id);
    action {
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("actseq"),
        actseq_args {
            .org = org,
            .agg_symbol = agg_symbol,
            .seq_ids = init_seq_ids
        }
    }.send();


}

ACTION boundagg::endseq(name org, symbol agg_symbol, vector<uint64_t> seq_ids) {
    string action_name = "endseq";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    if(!has_auth(get_self())) {
        check_internal_auth(name(action_name), failure_identifier);
    }

    // Access the aggdetail table
    aggdetail_table aggdetail_t(_self, org.value);
    auto aggdetail_itr = aggdetail_t.find(agg_symbol.code().raw());

    check(aggdetail_itr != aggdetail_t.end(), failure_identifier + "Aggregation symbol does not exist.");


    aggdetail_t.modify(aggdetail_itr, _self, [&](auto& row) {
        vector<uint64_t> active_seq_ids = row.active_seq_ids;
        vector<uint64_t> new_active_seq_ids;
        vector<uint64_t> end_seq_ids = row.end_seq_ids;
        


        for (uint64_t seq_id : active_seq_ids) {
            if (std::find(seq_ids.begin(), seq_ids.end(), seq_id) == seq_ids.end()) {
                new_active_seq_ids.push_back(seq_id);
            } else {
                end_seq_ids.push_back(seq_id);
            }
        }
        row.active_seq_ids = new_active_seq_ids;
        row.end_seq_ids = end_seq_ids;
    });
    
    sequence_table sequence_t(get_self(), agg_symbol.code().raw());
    for (auto& seq_id : seq_ids) {
        auto itr = sequence_t.find(seq_id);
        check(itr != sequence_t.end(), failure_identifier + "Sequence ID does not exist.");
        check(itr->seq_status == "active"_n, failure_identifier + "Sequence is not in active state.");

        sequence_t.modify(itr, _self, [&](auto& row) {
            row.seq_status = "end"_n;
            row.end_time = time_point_sec(current_time_point());
        });

        update_status_in_badgestatus(org, agg_symbol, seq_id, "end"_n, failure_identifier);
    }
}

ACTION boundagg::endseqaa(name org, symbol agg_symbol) {
    string action_name = "endseqaa";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    vector<uint64_t> active_seq_ids = all_active_seq_ids (org, agg_symbol, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("endseq"),
      endseq_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = active_seq_ids
      }
    }.send();
}

ACTION boundagg::endseqfa(name org, symbol agg_symbol) {
    string action_name = "endseqfa";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    vector<uint64_t> active_seq_ids;
    uint64_t first_seq_id = all_active_seq_ids (org, agg_symbol, failure_identifier).front();
    active_seq_ids.push_back(first_seq_id);
    action {
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("actseq"),
        actseq_args {
            .org = org,
            .agg_symbol = agg_symbol,
            .seq_ids = active_seq_ids
        }
    }.send();

}

ACTION boundagg::addbadge(name org, symbol agg_symbol, vector<uint64_t> seq_ids, vector<symbol> badge_symbols) {
    string action_name = "addbadge";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    if(!has_auth(get_self())) {
        check_internal_auth(name(action_name), failure_identifier);
    }

    sequence_table sequence_t(get_self(), agg_symbol.code().raw());
    for (const auto& seq_id : seq_ids) {
        auto itr = sequence_t.find(seq_id);
        check(itr != sequence_t.end(), failure_identifier + "Sequence ID does not exist.");
        for (const auto& badge_symbol : badge_symbols) {
            insert_record_in_badgestatus(org, agg_symbol, seq_id, badge_symbol, itr->seq_status, failure_identifier);
        }
    }
}

ACTION boundagg::addbadgefa(name org, symbol agg_symbol, vector<symbol> badge_symbols) {
    string action_name = "addbadgefa";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> active_seq_ids;
    uint64_t first_seq_id = all_active_seq_ids (org, agg_symbol, failure_identifier).front();
    active_seq_ids.push_back(first_seq_id);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addbadge"),
      addbadge_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = active_seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send(); 
}

ACTION boundagg::addbadgeaa(name org, symbol agg_symbol, vector<symbol> badge_symbols) {
    string action_name = "addbadgeaa";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> active_seq_ids = all_active_seq_ids (org, agg_symbol, failure_identifier);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addbadge"),
      addbadge_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = active_seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send(); 
}

ACTION boundagg::addbadgefi(name org, symbol agg_symbol, vector<symbol> badge_symbols) {
    string action_name = "addbadgefi";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> init_seq_ids;
    uint64_t first_seq_id = all_init_seq_ids (org, agg_symbol, failure_identifier).front();
    init_seq_ids.push_back(first_seq_id);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addbadge"),
      addbadge_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = init_seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send(); 
}

ACTION boundagg::addbadgeai(name org, symbol agg_symbol, vector<symbol> badge_symbols) {
    string action_name = "addbadgeai";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> init_seq_ids = all_init_seq_ids (org, agg_symbol, failure_identifier);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addbadge"),
      addbadge_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = init_seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send(); 

}

void boundagg::notifyachiev(name org, asset badge_asset, name from, name to, string memo, vector<name> notify_accounts) {
    string action_name = "notifyachiev";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";

    badgestatus_table badgestatus(get_self(), org.value);
    auto by_status_index = badgestatus.get_index<"bybadgestat"_n>();

    auto hashed_active_status = hash_active_status(badge_asset.symbol, "active"_n, "active"_n);
    achievements_table achievements(get_self(), to.value);

    auto itr = by_status_index.find(hashed_active_status);
    while(itr != by_status_index.end() && itr->badge_symbol == badge_asset.symbol && itr->badge_status == "active"_n && itr->seq_status == "active"_n) {
        
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
        
        // Move to the next record
        ++itr;
    }
}


    ACTION boundagg::pauseall(name org, symbol agg_symbol, uint64_t seq_id) {
        string action_name = "pauseall";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        if(!has_auth(get_self())) {
            check_internal_auth(name(action_name), failure_identifier);
        }
        update_badge_status_all(org, agg_symbol, seq_id, "paused"_n, failure_identifier);
    }

    ACTION boundagg::pausebadge(name org, symbol agg_symbol, uint64_t badge_agg_seq_id) {
        string action_name = "pausebadge";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        update_badge_status(org, agg_symbol, badge_agg_seq_id, "paused"_n, failure_identifier);

    }
    ACTION boundagg::pausebadges(name org, symbol agg_symbol, uint64_t seq_id, vector<symbol> badge_symbols) {
        string action_name = "pausebadges";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        if(!has_auth(get_self())) {
            check_internal_auth(name(action_name), failure_identifier);
        }
        update_badge_statuses(org, agg_symbol, seq_id, badge_symbols, "paused"_n, failure_identifier);
    }

    ACTION boundagg::pauseallfa(name org, symbol agg_symbol) {
        string action_name = "pauseallfa";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        uint64_t first_active_seq_id = all_active_seq_ids (org, agg_symbol, failure_identifier).front();
        action {
            permission_level{get_self(), name("active")},
            name(get_self()),
            name("pauseall"),
            pauseall_args {
                .org = org,
                .agg_symbol = agg_symbol,
                .seq_id = first_active_seq_id
            }
        }.send();     
    }

    ACTION boundagg::resumeall(name org, symbol agg_symbol, uint64_t seq_id) {
        string action_name = "resumeall";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        update_badge_status_all(org, agg_symbol, seq_id, "active"_n, failure_identifier);
    }
    ACTION boundagg::resumebadge(name org, symbol agg_symbol, uint64_t badge_agg_seq_id) {
        string action_name = "resumebadge";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        update_badge_status(org, agg_symbol, badge_agg_seq_id, "active"_n, failure_identifier);    
    }
    ACTION boundagg::resumebadges(name org, symbol agg_symbol, uint64_t seq_id, vector<symbol> badge_symbols) {
        string action_name = "resumebadges";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        if(!has_auth(get_self())) {
            check_internal_auth(name(action_name), failure_identifier);
        }
        update_badge_statuses(org, agg_symbol, seq_id, badge_symbols, "active"_n, failure_identifier);
    }


