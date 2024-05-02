#include <hllval.hpp>

void hllval::newemission(name authorized, 
    symbol badge_symbol, 
    vector<symbol> sender_uniqueness_badge_symbols) {

    string action_name = "newemission";
    string failure_identifier = "CONTRACT: hllval, ACTION: " + action_name + ", MESSAGE: ";

    name badge_name = get_name_from_internal_symbol(badge_symbol, failure_identifier);
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_badge_authority(org, name(action_name), badge_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");

}
    
void hllval::activate(name authorized,
      symbol badge_symbol) {

    string action_name = "activate";
    string failure_identifier = "CONTRACT: hllval, ACTION: " + action_name + ", MESSAGE: ";

    name badge_name = get_name_from_internal_symbol(badge_symbol, failure_identifier);
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_badge_authority(org, name(action_name), badge_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");

}

void hllval::deactivate(name authorized, 
    symbol badge_symbol) {

    string action_name = "deactivate";
    string failure_identifier = "CONTRACT: hllval, ACTION: " + action_name + ", MESSAGE: ";

    name badge_name = get_name_from_internal_symbol(badge_symbol, failure_identifier);
    name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_badge_authority(org, name(action_name), badge_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");    
}


ACTION hllval::addbadgeauth(name org, name action, name badge, name authorized_account) {
    require_auth (org);

    badgeauths_table badgeauths(get_self(), org.value);
    auto secondary_index = badgeauths.get_index<"actionemiss"_n>();
    uint128_t secondary_key = badgeauths::combine_names(action, badge);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->badge == badge, "badge with specified action and badge not found");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.push_back(authorized_account);
    });
}

ACTION hllval::delbadgeauth(name org, name action, name badge, name authorized_account) {
    require_auth (org);

    string action_name = "delbadgeauth";
    string failure_identifier = "CONTRACT: hllval, ACTION: " + action_name + ", MESSAGE: ";
 
    badgeauths_table badgeauths(get_self(), org.value);
    auto secondary_index = badgeauths.get_index<"actionemiss"_n>();
    uint128_t secondary_key = badgeauths::combine_names(action, badge);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->badge == badge, failure_identifier + "badge with specified action and badge not found");

    auto& accounts = itr->authorized_accounts;
    auto acc_itr = std::find(accounts.begin(), accounts.end(), authorized_account);
    check(acc_itr != accounts.end(), failure_identifier + "Account not found in authorized accounts");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.erase(acc_itr);
    });
}

ACTION hllval::addactionauth (name org, name action, name authorized_account) {
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

ACTION hllval::delactionauth (name org, name action, name authorized_account) {
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