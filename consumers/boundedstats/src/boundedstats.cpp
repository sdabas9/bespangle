#include <boundedstats.hpp>


void boundedstats::notifyachiev(name org, asset badge_asset, name from, name to, string memo, vector<name> notify_accounts) {
    string action_name = "settings";
    string failure_identifier = "CONTRACT: boundedstats, ACTION: " + action_name + ", MESSAGE: ";
    badgestatus_table badgestatus(name(BOUNDED_AGG_CONTRACT), org.value); // Adjust scope as necessary
    auto badge_status_index = badgestatus.get_index<"bybadgestat"_n>(); // Correct index name
    auto hashed_active_status = hash_active_status(badge_asset.symbol, "active"_n, "active"_n);
    auto itr = badge_status_index.find(hashed_active_status);
    while(itr != badge_status_index.end() && itr->badge_symbol == badge_asset.symbol && itr->badge_status == "active"_n && itr->seq_status == "active"_n) {
        uint64_t new_balance = get_new_balance(to, itr->badge_agg_seq_id);
        update_rank(org, to, itr->badge_agg_seq_id, new_balance - badge_asset.amount, new_balance);
        counts_table _counts(get_self(), org.value);
        auto counts_itr = _counts.find(itr->badge_agg_seq_id);
        uint64_t total_recipients;
        uint64_t total_issued;
        if(counts_itr == _counts.end()) {
            total_recipients = 1;
            total_issued = badge_asset.amount;
            _counts.emplace(get_self(), [&](auto& entry) {
                entry.badge_agg_seq_id = itr->badge_agg_seq_id;
                entry.total_recipients = total_recipients;
                entry.total_issued = total_issued;
            });
        } else {
            total_recipients = counts_itr->total_recipients;
            if(new_balance == badge_asset.amount) {
                total_recipients++;
            }
            total_issued = counts_itr->total_issued + badge_asset.amount;
            _counts.modify(counts_itr, get_self(), [&](auto& entry) {
                entry.total_recipients = total_recipients;
                entry.total_issued = total_issued;
            });
        }
        itr++;
    }
}

ACTION boundedstats::settings(name org, symbol agg_symbol, uint64_t seq_id, symbol badge_symbol, uint64_t max_no_of_ranks) {
    string action_name = "settings";
    string failure_identifier = "CONTRACT: boundedstats, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    badgestatus_table badgestatus(name(BOUNDED_AGG_CONTRACT), org.value); // Adjust scope as necessary
    auto index = badgestatus.get_index<"aggseqbadge"_n>(); // Correct index name
    auto key = hash_agg_seq_badge(agg_symbol, seq_id, badge_symbol);
    auto itr = index.find(key);
    if(itr != index.end() && itr->agg_symbol == agg_symbol && itr->seq_id == seq_id && itr->badge_symbol == badge_symbol) {
        statssetting_table _statssetting(get_self(), org.value);
        auto stats_itr = _statssetting.find(itr->badge_agg_seq_id);
        check(stats_itr == _statssetting.end(), "record already exist in statssetting table");
        _statssetting.emplace(get_self(), [&](auto& entry) {
            entry.badge_agg_seq_id = itr->badge_agg_seq_id;
            entry.max_no_of_ranks = max_no_of_ranks;
            entry.current_no_of_ranks = 0;
        });        
    } else {
        check(false, failure_identifier + "no record in badgestatus table with agg_symbol, seq_id and badge_symbol");    
    }
}
