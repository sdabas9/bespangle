#include <abval.hpp>

void abval::create(name authorized, 
            name org, 
            name anti_badge,
            name badge,
            name type,
            string offchain_lookup_data,
            string onchain_lookup_data,
            vector<name> consumers,
            string memo) {
    string action_name = "create";
    string failure_identifier = "CONTRACT: abval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_badge_authority(org, name(action_name), badge, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void abval::createinv(name authorized, 
               name org, 
               name anti_badge,
               name badge,
               string offchain_lookup_data,
               string onchain_lookup_data,
               vector<name> consumers,
               string memo) {
    string action_name = "createinv";
    string failure_identifier = "CONTRACT: abval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_badge_authority(org, name(action_name), badge, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void abval::issue(name authorized, 
           name org,
           name anti_badge, 
           uint64_t amount,
           name to, 
           string memo) {
    string action_name = "issue";
    string failure_identifier = "CONTRACT: abval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_badge_authority(org, name(action_name), anti_badge, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

ACTION abval::addbadgeauth(name org, name action, name badge, name authorized_account) {
    require_auth(org);

    badgeauths_table badgeauths(get_self(), org.value);
    auto secondary_index = badgeauths.get_index<"byactionbadge"_n>();
    uint128_t secondary_key = badgeauths::combine_names(action, badge);
    auto itr = secondary_index.find(secondary_key);

    if (itr != secondary_index.end() && itr->action == action && itr->badge == badge) {
        bool is_account_already_authorized = find(itr->authorized_accounts.begin(), itr->authorized_accounts.end(), authorized_account) != itr->authorized_accounts.end();
        check(!is_account_already_authorized, "Account is already authorized for this badge and action.");

        badgeauths.modify(*itr, get_self(), [&](auto& row) {
            row.authorized_accounts.push_back(authorized_account);
        });
    } else {
        badgeauths.emplace(get_self(), [&](auto& row) {
            row.id = badgeauths.available_primary_key();
            row.action = action;
            row.badge = badge;
            row.authorized_accounts.push_back(authorized_account);
        });
    }
}

ACTION abval::delbadgeauth(name org, name action, name badge, name authorized_account) {
    require_auth(org);

    string action_name = "delbadgeauth";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";
 
    badgeauths_table badgeauths(get_self(), org.value);
    auto secondary_index = badgeauths.get_index<"byactionbadge"_n>();
    uint128_t secondary_key = badgeauths::combine_names(action, badge);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->badge == badge, failure_identifier + "badge with specified action and badge not found");

    auto acc_itr = find(itr->authorized_accounts.begin(), itr->authorized_accounts.end(), authorized_account);
    check(acc_itr != itr->authorized_accounts.end(), failure_identifier + "Account not found in authorized accounts");
    
    bool should_erase = false;
    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.erase(acc_itr);
        should_erase = row.authorized_accounts.empty();
    });

    if (should_erase) {
        secondary_index.erase(itr);
    }
}

ACTION abval::addactionauth(name org, name action, name authorized_account) {
    require_auth(org);

    actionauths_table _actionauths(get_self(), org.value);
    auto itr = _actionauths.find(action.value);

    if (itr != _actionauths.end()) {
        _actionauths.modify(itr, get_self(), [&](auto& row) {
            auto it = find(row.authorized_accounts.begin(), row.authorized_accounts.end(), authorized_account);
            if (it == row.authorized_accounts.end()) {
                row.authorized_accounts.push_back(authorized_account);
            } else {
                check(false, "Account is already authorized for this action.");
            }
        });
    } else {
        _actionauths.emplace(get_self(), [&](auto& row) {
            row.action = action;
            row.authorized_accounts.push_back(authorized_account);
        });
    }
}

ACTION abval::delactionauth(name org, name action, name authorized_account) {
    require_auth(org);

    actionauths_table _actionauths(get_self(), org.value);
    auto itr = _actionauths.find(action.value);

    if (itr != _actionauths.end()) {
        bool should_erase = false;

        _actionauths.modify(itr, get_self(), [&](auto& row) {
            auto it = find(row.authorized_accounts.begin(), row.authorized_accounts.end(), authorized_account);
            if (it != row.authorized_accounts.end()) {
                row.authorized_accounts.erase(it);
            }
            
            should_erase = row.authorized_accounts.empty();
        });

        if (should_erase) {
            _actionauths.erase(itr);
        }
    }
    check(false, "account already not authorized");
}
