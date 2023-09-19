#include <checks.hpp>

ACTION checks::ownnotown(name org, name account, vector<sbadges> own, vector<sbadges> not_own) {
    for (auto i = 0; i < own.size(); i++) {
        own_check(org, account, own[i].badge, own[i].count);
    }
    for (auto i = 0; i < not_own.size(); i++) {
        not_own_check(org, account, not_own[i].badge, not_own[i].count);
    }
}

ACTION checks::lookback(name org, name account, name series, uint8_t lookback, uint8_t musthave) {
    check(musthave <= lookback, "UNSUPPORTED INPUT: musthave cannot be greater than lookback");
    check(lookback > 0, "UNSUPPORTED INPUT: lookback should be at least 1");
    if (musthave == 0) {
        return;
    }
    uint64_t last_seq_id = latest_seq_id(org, series);
    vector<uint64_t> sequences;
    sequences.push_back(last_seq_id);
    for (auto i = 0; i < lookback - 1; i++) {
        last_seq_id = last_seq_id - 1;
        if (last_seq_id > 0) {
            sequences.push_back(last_seq_id);
        }
    }
    uint8_t hascount = series_own_count(org, account, series, sequences);
    if (hascount >= musthave) {
        return;
    } else {
        check(false, "CHECK FAILED: account has only " + to_string(hascount) + "; needed " + to_string(musthave));
    }
}

// todo call lookback action with musthave 1 and lookback 1
ACTION checks::haslatest(name org, name account, name series) {
    uint64_t last_seq_id = latest_seq_id(org, series);
    check(last_seq_id > 0, "CHECK FAILED: no badge defined in series");
    vector<uint64_t> sequences;
    sequences.push_back(last_seq_id);
    uint8_t hascount = series_own_count(org, account, series, sequences);
    if (hascount == 1) {
        return;
    } else {
        check(false, "CHECK FAILED: account does not have the latest in series");
    }
}

ACTION checks::canissueanti(name org, name account, name antibadge, uint64_t amount) {
    // badge in antibadge is still valid post issuance of this amount.
    name badge = get_badge_for_antibadge(org, antibadge);
    vector<name> antibadges = anti_badge_vector(org, badge);
    uint64_t total_antibadge_balance = multiple_badges_additive_balance(org, account, antibadges);
    uint64_t badge_balance = account_balance(org, account, badge);
    check(total_antibadge_balance + amount <= badge_balance, "CHECK FAILED: cumulative balance of all antibadges will exceed the badge balance");
}

ACTION checks::issuebndanti(name org, name antibadge, name account, uint64_t amount) {
    name badge = get_badge_for_antibadge(org, antibadge);
    vector<name> antibadges = anti_badge_vector(org, badge);
    // get rounds that badge has subscribed to
    vector<name> ongoing_rounds = badge_rounds(org, badge);
    for(auto i = 0; i < ongoing_rounds.size(); i++) {
        vector<uint64_t> badgeround_ids = relevant_badgeround_ids(org, ongoing_rounds[i], antibadges);
        check(badgeround_ids.size()>0, "antibadge not active or setup for any rounds");
        uint64_t badge_round_balance = bounded_account_balance(org, ongoing_rounds[i], badge, account);
        uint64_t total_round_antibadge_balance = bounded_multiple_badges_additive_balance (org, badgeround_ids, account);
        check(total_round_antibadge_balance + amount <= badge_round_balance, "CHECK FAILED: cumulative balance of all antibadges will exceed the badge balance for round <>");
    }

}


