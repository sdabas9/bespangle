#include <statistics.hpp>


void statistics::notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts) {

    string action_name = "notifyachiev";
    string failure_identifier = "CONTRACT: statistics, ACTION: " + action_name + ", MESSAGE: ";
    accounts _accounts(name(CUMULATIVE_CONTRACT), to.value);
    auto accounts_itr = _accounts.find(badge_asset.symbol.code().raw());
    
    uint64_t new_balance = (accounts_itr == _accounts.end()) ? 0 : accounts_itr->balance.amount;
    update_rank(org, to, badge_asset.symbol, new_balance - badge_asset.amount, new_balance);
    
    counts_table _counts(get_self(), org.value);
    auto counts_itr = _counts.find(badge_asset.symbol.code().raw());
    uint64_t total_recipients;
    uint64_t total_issued;
    if(counts_itr == _counts.end()) {
        total_recipients = 1;
        total_issued = badge_asset.amount;
        _counts.emplace(get_self(), [&](auto& entry) {
            entry.badge_symbol = badge_asset.symbol;
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
}

ACTION statistics::settings(name org, symbol badge_symbol, uint64_t max_no_of_ranks) {
    string action_name = "settings";
    string failure_identifier = "CONTRACT: statistics, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);
      
    badge_table _badges(name(ORCHESTRATOR_CONTRACT), org.value); // Adjust scope as necessary
    auto itr = _badges.find(badge_symbol.code().raw());
    
    if(itr != _badges.end()) {
        statssetting_table _statssetting(get_self(), org.value);
        auto stats_itr = _statssetting.find(badge_symbol.code().raw());
        check(stats_itr == _statssetting.end(), "record already exist in statssetting table");
        _statssetting.emplace(get_self(), [&](auto& entry) {
            entry.badge_symbol = badge_symbol;
            entry.max_no_of_ranks = max_no_of_ranks;
            entry.current_no_of_ranks = 0;
        });        
    } else {
        check(false, failure_identifier + "no record in badge table with badge_symbol");    
    }
}

