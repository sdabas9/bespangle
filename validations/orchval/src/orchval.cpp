#include <orchval.hpp>

void orchval::delfeature (name org,
    name authorized, 
    name badge,
    name consumer,
    string memo) {

    string action_name = "delfeature";
    string failure_identifier = "CONTRACT: orchval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_badge_authority(org, name(action_name), badge, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");

}

ACTION orchval::addbadgeauth(name org, name action, name badge, name authorized_account) {
    require_auth (org);

    badgeauths_table badgeauths(get_self(), org.value);
    auto secondary_index = badgeauths.get_index<"byactionbadge"_n>();
    uint128_t secondary_key = badgeauths::combine_names(action, badge);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->badge == badge, "Badge with specified action and badge not found");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.push_back(authorized_account);
    });
}

ACTION orchval::delbadgeauth(name org, name action, name badge, name authorized_account) {
    require_auth (org);

    string action_name = "delbadgeauth";
    string failure_identifier = "CONTRACT: orchval, ACTION: " + action_name + ", MESSAGE: ";
 
    badgeauths_table badgeauths(get_self(), org.value);
    auto secondary_index = badgeauths.get_index<"byactionbadge"_n>();
    uint128_t secondary_key = badgeauths::combine_names(action, badge);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->badge == badge, failure_identifier + "Badge with specified action and badge not found");

    auto& accounts = itr->authorized_accounts;
    auto acc_itr = std::find(accounts.begin(), accounts.end(), authorized_account);
    check(acc_itr != accounts.end(), failure_identifier + "Account not found in authorized accounts");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.erase(acc_itr);
    });
}

ACTION orchval::addactionauth (name org, name action, name authorized_account) {
  require_auth (org);
  actionauths_table _actionauths(get_self(), org.value);
  auto itr = _actionauths.find(action.value);
  if (itr != _actionauths.end()) {
      _actionauths.modify(itr, get_self(), [&](auto& row) {
          row.authorized_accounts.push_back(authorized_account);
      });
  } else {
      _actionauths.emplace(get_self(), [&](auto& row) {
          row.action = action;
          row.authorized_accounts.push_back(authorized_account);
      });
  }
}

ACTION orchval::delactionauth (name org, name action, name authorized_account) {
  require_auth (org);
  actionauths_table _actionauths(get_self(), org.value);
  auto itr = _actionauths.find(action.value);
  if (itr != _actionauths.end()) {
      _actionauths.modify(itr, get_self(), [&](auto& row) {
          auto it = std::find(row.authorized_accounts.begin(), row.authorized_accounts.end(), authorized_account);
          if (it != row.authorized_accounts.end()) {
              row.authorized_accounts.erase(it);
          }
      });
  }
}