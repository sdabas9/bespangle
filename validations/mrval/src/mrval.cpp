#include <mrval.hpp>

void mrval::init(
    name authorized,
    symbol badge_symbol,
    time_point_sec starttime,
    uint64_t cycle_length,
    uint8_t supply_per_cycle,
    string offchain_lookup_data,
    string onchain_lookup_data,
    bool lifetime_aggregate,
    bool lifetime_stats,
    string memo
) {
    string action_name = "init";
    string failure_identifier = "CONTRACT: mrval, ACTION: " + action_name + ", MESSAGE: ";

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

void mrval::starttime(
    name authorized,
    symbol badge_symbol,
    time_point_sec new_starttime
) {
    string action_name = "starttime";
    string failure_identifier = "CONTRACT: mrval, ACTION: " + action_name + ", MESSAGE: ";

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

void mrval::cyclelength(
    name authorized,
    symbol badge_symbol,
    uint64_t new_cycle_length
) {
    string action_name = "cyclelength";
    string failure_identifier = "CONTRACT: mrval, ACTION: " + action_name + ", MESSAGE: ";

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
void mrval::cyclesupply(
    name authorized,
    symbol badge_symbol,
    uint8_t new_supply_per_cycle
) {
    string action_name = "cyclesupply";
    string failure_identifier = "CONTRACT: mrval, ACTION: " + action_name + ", MESSAGE: ";

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


ACTION mrval::addbadgeauth(name org, name action, name badge, name authorized_account) {
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

ACTION mrval::delbadgeauth(name org, name action, name badge, name authorized_account) {
    require_auth (org);

    string action_name = "delbadgeauth";
    string failure_identifier = "CONTRACT: mrval, ACTION: " + action_name + ", MESSAGE: ";
 
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

ACTION mrval::addactionauth (name org, name action, name authorized_account) {
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

ACTION mrval::delactionauth (name org, name action, name authorized_account) {
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