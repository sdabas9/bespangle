#include <redeem.hpp>
#include <sstream>


ACTION redeem::fixedlifetime() {
 // only fixed reward
}

ACTION redeem::fixedfa() {
    // first active season
}

ACTION redeem::dynamiclifetime() {
    // 
}

ACTION redeem::dynamicfa() {
    //
}

ACTION redeem::initfixed(name org, name round, name badge, uint64_t rate, name notification_contract) {
    require_auth(get_self()); // Only contract account can run this action
    
    // Check if rate is valid (according to your business logic)
    check(rate > 0, "Rate must be greater than zero");

    // Ensure the notification contract is valid
    check(is_account(notification_contract), "Notification contract account does not exist");

    uint64_t new_id = get_next_id(); // Get next id

    fixed_table f_t(get_self(), org.value);
    auto idx = f_t.get_index<name("byroundbadge")>();
    auto existing = idx.find((uint128_t(round.value) << 64) | badge.value);

    if (existing == idx.end() || existing->round != round || existing->badge != badge) { // If not exist, create new entry
        f_t.emplace(get_self(), [&](auto& f) {
            f.id = new_id;
            f.round = round;
            f.badge = badge;
            f.contract = notification_contract;
            f.rate = rate;
        });
    } else { // If exist, modify the existing entry
        f_t.modify(*existing, get_self(), [&](auto& f) {
            f.rate = rate;
            f.contract = notification_contract;
        });
    }
}

ACTION redeem::initdynamic(name org, name round, name badge, uint64_t amount, name notification_contract) {
    require_auth(get_self()); // Only contract account can run this action
    
    // Check if amount is valid (according to your business logic)
    check(amount > 0, "Amount must be greater than zero");

    // Ensure the notification contract is valid
    check(is_account(notification_contract), "Notification contract account does not exist");

    uint64_t new_id = get_next_id(); // Get next id

    dynamic_table d_t(get_self(), org.value);
    auto idx = d_t.get_index<name("byroundbadge")>();
    auto existing = idx.find((uint128_t(round.value) << 64) | badge.value);

    if (existing == idx.end() || existing->round != round || existing->badge != badge) { // If not exist, create new entry
        d_t.emplace(get_self(), [&](auto& d) {
            d.id = new_id;
            d.round = round;
            d.badge = badge;
            d.contract = notification_contract;
            d.amount = amount;
        });
    } else { // If exist, modify the existing entry
        d_t.modify(*existing, get_self(), [&](auto& d) {
            d.amount = amount;
            d.contract = notification_contract;
        });
    }
}

ACTION redeem::distfixed(name org, name round, name badge, name account, uint64_t amount_val, name notification_contract) {
    require_auth(get_self());
    require_recipient(notification_contract);
}

ACTION redeem::distdynamic(name org, name round, name badge, name account, uint64_t portion, name notification_contract) {
    require_auth(get_self());
    require_recipient(notification_contract);
}


ACTION redeem::redeemfix(name org, name account, name round, name badge) {
    require_auth(get_self());
    vector<name> antibadges = anti_badge_vector(org, badge);
    vector<uint64_t> badgeround_ids = relevant_badgeround_ids(org, round, antibadges);
    check(badgeround_ids.size()>0, "antibadge not active or setup for this round");
    uint64_t badge_round_balance = bounded_account_balance(org, round, badge, account);
    uint64_t total_round_antibadge_balance = bounded_multiple_badges_additive_balance (org, badgeround_ids, account);
    check(badge_round_balance>total_round_antibadge_balance, "not enough to redeem");
    uint64_t badge_amount = badge_round_balance - total_round_antibadge_balance;
    // Fetch the rate, id and contract name from the fixed table
    fixed_table f_t(get_self(), org.value);
    auto idx = f_t.get_index<name("byroundbadge")>();
    auto existing = idx.find((uint128_t(round.value) << 64) | badge.value);
    check(existing != idx.end(), "No entry found in fixed table");

    uint64_t rate = existing->rate;
    name notification_contract = existing->contract;
    uint64_t transfer_amount = rate * badge_amount;
    // get invalidated balance
    // get redeemed balance
    name redeemable_badge_name = redeem_badge(org, badge);

    action(
        permission_level{get_self(), "active"_n},
        name(ANTIBADGE_CONTRACT),
        "issue"_n,
        issuesbt_args{
            .org = org,
            .to = account,
            .antibadge = redeemable_badge_name,
            .amount = badge_amount,
            .memo = "redeemed"
        }
    ).send();

    action(
        permission_level{get_self(), "active"_n},
        get_self(),
        "distfixed"_n,
        distfixed_args{
            .org = org,
            .round = round,
            .badge = badge,
            .account = account,
            .amount_val = transfer_amount,
            .notification_contract = notification_contract
        }
    ).send();
}


ACTION redeem::redeemdyn(name org, name account, name badge, name round) {
    require_auth(get_self());
    vector<name> antibadges = anti_badge_vector(org, badge);
    vector<uint64_t> badgeround_ids = relevant_badgeround_ids(org, round, antibadges);
    check(badgeround_ids.size()>0, "antibadge not active or setup for this round");
    uint64_t badge_round_balance = bounded_account_balance(org, round, badge, account);
    uint64_t total_round_antibadge_balance = bounded_multiple_badges_additive_balance (org, badgeround_ids, account);
    check(badge_round_balance>total_round_antibadge_balance, "not enough to redeem");
    
    uint64_t badge_amount = badge_round_balance - total_round_antibadge_balance;
        // Fetch data from the keystats table
    keystats_table k_t(get_self(), org.value);
    auto k_idx = k_t.get_index<name("byroundbadge")>();
    auto k_existing = k_idx.find((uint128_t(round.value) << 64) | badge.value);
    check(k_existing != k_idx.end(), "No entry found in keystats table");

    // Fetch data from the dynamic table
    dynamic_table d_t(get_self(), org.value);
    auto d_idx = d_t.get_index<name("byroundbadge")>();
    auto d_existing = d_idx.find((uint128_t(round.value) << 64) | badge.value);
    check(d_existing != d_idx.end(), "No entry found in dynamic table");

    // Calculating transfer_amount
    double ratio = static_cast<double>(badge_amount) / static_cast<double>(k_existing->total);
    uint64_t transfer_portion = static_cast<uint64_t>(ratio * d_existing->amount);

    // Fetching contract_account and id from dynamic table
    name notification_contract = d_existing->contract;

    name redeemable_badge_name = redeem_badge(org, badge);

    action(
        permission_level{get_self(), "active"_n},
        name(ANTIBADGE_CONTRACT),
        "issue"_n,
        issuesbt_args{
            .org = org,
            .to = account,
            .antibadge = redeemable_badge_name,
            .amount = badge_amount,
            .memo = "redeemed"
        }
    ).send();

    action(
        permission_level{get_self(), "active"_n},
        get_self(),
        "distdynamic"_n,
        distdynamic_args{
            .org = org,
            .round = round,
            .badge = badge,
            .account = account,
            .portion = transfer_portion,
            .notification_contract = notification_contract
        }
    ).send();

}



