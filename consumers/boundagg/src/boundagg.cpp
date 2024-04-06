#include <boundagg.hpp>

ACTION boundagg::initagg(symbol agg_symbol, string first_seq_description) {
    string action_name = "initagg";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    // Access the agginfo table
    agginfo_table agginfo_t(_self, org.value);
    auto agginfo_it = agginfo_t.find(agg_symbol.code().raw());

    uint64_t next_seq_id = 1;
    if (agginfo_it != agginfo_t.end()) {
        next_seq_id = agginfo_it->last_init_seq_id + 1;
        agginfo_t.modify(agginfo_it, _self, [&](auto& row) {
            row.last_init_seq_id = next_seq_id;
            row.init_seq_ids.push_back(next_seq_id);
        });
    } else {
        agginfo_t.emplace(_self, [&](auto& row) {
            row.agg_symbol = agg_symbol;
            row.last_init_seq_id = next_seq_id;
            row.init_seq_ids.push_back(next_seq_id);
        });
    }

    // Insert into agg table
    agg_table agg_t(_self, agg_symbol.code().raw());
    agg_t.emplace(_self, [&](auto& row) {
        row.seq_id = next_seq_id;
        row.seq_status = "init"_n;
        row.description = first_seq_description;
        row.init_time = time_point_sec(current_time_point());
        // active_time and end_time will be set in future actions
    });
}

ACTION boundagg::initseq(symbol agg_symbol, string description) {
    string action_name = "initseq";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    // Access the agginfo table
    agginfo_table agginfo_t(_self, org.value);
    auto agginfo_itr = agginfo_t.find(agg_symbol.code().raw());

    check(agginfo_itr != agginfo_t.end(), failure_identifier + "Aggregation symbol does not exist.");

    // Update the last_init_seq_id in agginfo table
    agginfo_t.modify(agginfo_itr, _self, [&](auto& row) {
        row.last_init_seq_id += 1;
        row.init_seq_ids.push_back(row.last_init_seq_id);
    });

    // Insert new entry in the agg table
    agg_table agg_t(get_self(), agg_symbol.code().raw()); // Scope to agg_symbol.code().raw()
    agg_t.emplace(_self, [&](auto& row) {
        row.seq_id = agginfo_itr->last_init_seq_id;
        row.seq_status = "init"_n;
        row.description = description;
        row.init_time = time_point_sec(current_time_point());
        // The active_time and end_time are left uninitialized here and will be set later
    });
}

ACTION boundagg::actseq(symbol agg_symbol, vector<uint64_t> seq_ids) {
    string action_name = "actseq";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    if(!has_auth(get_self())) {
        check_internal_auth(name(action_name), failure_identifier);
    }
    

    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    // Access the agginfo table
    agginfo_table agginfo_t(_self, org.value);
    auto agginfo_itr = agginfo_t.find(agg_symbol.code().raw());

    check(agginfo_itr != agginfo_t.end(), failure_identifier + "Aggregation symbol does not exist.");

    // Update the last_init_seq_id in agginfo table
    agginfo_t.modify(agginfo_itr, _self, [&](auto& row) {
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
    
    agg_table agg_t(get_self(), agg_symbol.code().raw());
    for (auto& seq_id : seq_ids) {
        auto itr = agg_t.find(seq_id);
        check(itr != agg_t.end(), failure_identifier + "Sequence ID does not exist.");
        check(itr->seq_status == "init"_n, failure_identifier + "Sequence is not in init state.");

        agg_t.modify(itr, _self, [&](auto& row) {
            row.seq_status = "active"_n;
            row.active_time = time_point_sec(current_time_point());
        });
        update_status_in_badgeaggseq(agg_symbol, seq_id, "active"_n);
    }

}

ACTION boundagg::actseqai(symbol agg_symbol) {
    string action_name = "actseqai";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    vector<uint64_t> init_seq_ids = all_init_seq_ids (agg_symbol, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("actseq"),
      actseq_args {
        .agg_symbol = agg_symbol,
        .seq_ids = init_seq_ids
      }
    }.send();

}

ACTION boundagg::actseqfi(symbol agg_symbol) {
    string action_name = "actseqfi";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> init_seq_ids;
    uint64_t first_seq_id = all_init_seq_ids (agg_symbol, failure_identifier).front();
    init_seq_ids.push_back(first_seq_id);
    action {
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("actseq"),
        actseq_args {
            .agg_symbol = agg_symbol,
            .seq_ids = init_seq_ids
        }
    }.send();


}

ACTION boundagg::endseq(symbol agg_symbol, vector<uint64_t> seq_ids) {
    string action_name = "endseq";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    if(!has_auth(get_self())) {
        check_internal_auth(name(action_name), failure_identifier);
    }

    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    // Access the agginfo table
    agginfo_table agginfo_t(_self, org.value);
    auto agginfo_itr = agginfo_t.find(agg_symbol.code().raw());

    check(agginfo_itr != agginfo_t.end(), failure_identifier + "Aggregation symbol does not exist.");


    agginfo_t.modify(agginfo_itr, _self, [&](auto& row) {
        vector<uint64_t> active_seq_ids = row.active_seq_ids;
        vector<uint64_t> new_active_seq_ids;

        for (uint64_t seq_id : active_seq_ids) {
            if (std::find(seq_ids.begin(), seq_ids.end(), seq_id) == seq_ids.end()) {
                new_active_seq_ids.push_back(seq_id);
            }
        }
        row.active_seq_ids = new_active_seq_ids;
    });
    
    agg_table agg_t(get_self(), agg_symbol.code().raw());
    for (auto& seq_id : seq_ids) {
        auto itr = agg_t.find(seq_id);
        check(itr != agg_t.end(), failure_identifier + "Sequence ID does not exist.");
        check(itr->seq_status == "active"_n, failure_identifier + "Sequence is not in active state.");

        agg_t.modify(itr, _self, [&](auto& row) {
            row.seq_status = "end"_n;
            row.end_time = time_point_sec(current_time_point());
        });

        update_status_in_badgeaggseq(agg_symbol, seq_id, "end"_n);
    }
}

ACTION boundagg::endseqaa(symbol agg_symbol) {
    string action_name = "endseqaa";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    vector<uint64_t> active_seq_ids = all_active_seq_ids (agg_symbol, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("endseq"),
      endseq_args {
        .agg_symbol = agg_symbol,
        .seq_ids = active_seq_ids
      }
    }.send();
}

ACTION boundagg::endseqfa(symbol agg_symbol) {
    string action_name = "endseqfa";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    vector<uint64_t> active_seq_ids;
    uint64_t first_seq_id = all_active_seq_ids (agg_symbol, failure_identifier).front();
    active_seq_ids.push_back(first_seq_id);
    action {
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("actseq"),
        actseq_args {
            .agg_symbol = agg_symbol,
            .seq_ids = active_seq_ids
        }
    }.send();

}

ACTION boundagg::addbadge(symbol agg_symbol, vector<uint64_t> seq_ids, vector<symbol> badge_symbols) {
    string action_name = "addbadge";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    if(!has_auth(get_self())) {
        check_internal_auth(name(action_name), failure_identifier);
    }

    agg_table agg_t(get_self(), agg_symbol.code().raw());
    for (const auto& seq_id : seq_ids) {
        auto itr = agg_t.find(seq_id);
        check(itr != agg_t.end(), failure_identifier + "Sequence ID does not exist.");
        for (const auto& badge_symbol : badge_symbols) {
            insert_record_in_badgeaggseq(agg_symbol, seq_id, badge_symbol, itr->seq_status);
        }
    }
}

ACTION boundagg::addbadgefa(symbol agg_symbol, vector<symbol> badge_symbols) {
    string action_name = "addbadgefa";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> active_seq_ids;
    uint64_t first_seq_id = all_active_seq_ids (agg_symbol, failure_identifier).front();
    active_seq_ids.push_back(first_seq_id);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addbadge"),
      addbadge_args {
        .agg_symbol = agg_symbol,
        .seq_ids = active_seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send(); 
}

ACTION boundagg::addbadgeaa(symbol agg_symbol, vector<symbol> badge_symbols) {
    string action_name = "addbadgeaa";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> active_seq_ids = all_active_seq_ids (agg_symbol, failure_identifier);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addbadge"),
      addbadge_args {
        .agg_symbol = agg_symbol,
        .seq_ids = active_seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send(); 
}

ACTION boundagg::addbadgefi(symbol agg_symbol, vector<symbol> badge_symbols) {
    string action_name = "addbadgefi";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> init_seq_ids;
    uint64_t first_seq_id = all_init_seq_ids (agg_symbol, failure_identifier).front();
    init_seq_ids.push_back(first_seq_id);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addbadge"),
      addbadge_args {
        .agg_symbol = agg_symbol,
        .seq_ids = init_seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send(); 
}

ACTION boundagg::addbadgeai(symbol agg_symbol, vector<symbol> badge_symbols) {
    string action_name = "addbadgeai";
    string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
    vector<uint64_t> init_seq_ids = all_init_seq_ids (agg_symbol, failure_identifier);
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addbadge"),
      addbadge_args {
        .agg_symbol = agg_symbol,
        .seq_ids = init_seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send(); 

}

void boundagg::notifyachiev(asset badge_asset, name from, name to, string memo, vector<name> notify_accounts) {
    badgeaggseq_table badgeaggseqs(get_self(), get_self().value);
    auto by_status_index = badgeaggseqs.get_index<"bybadgestat"_n>();

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


    ACTION boundagg::pauseall(symbol agg_symbol, uint64_t seq_id) {
        string action_name = "pauseall";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        if(!has_auth(get_self())) {
            check_internal_auth(name(action_name), failure_identifier);
        }
        update_badge_status_all(agg_symbol, seq_id, "paused"_n);
    }

    ACTION boundagg::pausebadge(symbol agg_symbol, uint64_t badge_agg_seq_id) {
        string action_name = "pausebadge";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        update_badge_status(agg_symbol, badge_agg_seq_id, "paused"_n, failure_identifier);

    }
    ACTION boundagg::pausebadges(symbol agg_symbol, uint64_t seq_id, vector<symbol> badge_symbols) {
        string action_name = "pausebadges";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        if(!has_auth(get_self())) {
            check_internal_auth(name(action_name), failure_identifier);
        }
        update_badge_statuses(agg_symbol, seq_id, badge_symbols, "paused"_n);
    }

    ACTION boundagg::pauseallfa(symbol agg_symbol) {
        string action_name = "pauseallfa";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        uint64_t first_active_seq_id = all_active_seq_ids (agg_symbol, failure_identifier).front();
        action {
            permission_level{get_self(), name("active")},
            name(get_self()),
            name("pauseall"),
            pauseall_args {
                .agg_symbol = agg_symbol,
                .seq_id = first_active_seq_id
            }
        }.send();     
    }

    ACTION boundagg::resumeall(symbol agg_symbol, uint64_t seq_id) {
        string action_name = "resumeall";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        update_badge_status_all(agg_symbol, seq_id, "active"_n);
    }
    ACTION boundagg::resumebadge(symbol agg_symbol, uint64_t badge_agg_seq_id) {
        string action_name = "resumebadge";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        check_internal_auth(name(action_name), failure_identifier);
        update_badge_status(agg_symbol, badge_agg_seq_id, "active"_n, failure_identifier);    
    }
    ACTION boundagg::resumebadges(symbol agg_symbol, uint64_t seq_id, vector<symbol> badge_symbols) {
        string action_name = "resumebadges";
        string failure_identifier = "CONTRACT: boundagg, ACTION: " + action_name + ", MESSAGE: ";
        if(!has_auth(get_self())) {
            check_internal_auth(name(action_name), failure_identifier);
        }
        update_badge_statuses(agg_symbol, seq_id, badge_symbols, "active"_n);
    }


