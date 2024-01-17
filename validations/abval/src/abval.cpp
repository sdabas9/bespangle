#include <abval.hpp>

void abval::create(name org, name authorized,
  name antibadge,
  name badge,
  name type,
  string offchain_lookup_data,
  string onchain_lookup_data,
  vector<name> consumers,
  string memo) {

    if (has_action_authority(org, name("create"), authorized)) {
        return;
    }
    if (has_badge_authority(org, name("create"), badge, authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");

}

void abval::createinv(name org, name authorized,
  name antibadge,
  name badge,
  string offchain_lookup_data,
  string onchain_lookup_data,
  string memo) {

    if (has_action_authority(org, name("createinv"), authorized)) {
        return;
    }
    if (has_badge_authority(org, name("createinv"), badge, authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");

}

void abval::issue(name org, name authorized,
  name to,
  name antibadge,
  uint64_t amount,
  string memo) {

    if (has_action_authority(org, name("issue"), authorized)) {
        return;
    }
    if (has_badge_authority(org, name("issue"), antibadge, authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");

}


ACTION abval::addbadgeauth(name org, name action, name badge, name authorized_account) {
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

ACTION abval::delbadgeauth(name org, name action, name badge, name authorized_account) {
    require_auth (org);

    badgeauths_table badgeauths(get_self(), org.value);
    auto secondary_index = badgeauths.get_index<"byactionbadge"_n>();
    uint128_t secondary_key = badgeauths::combine_names(action, badge);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->badge == badge, "Badge with specified action and badge not found");

    auto& accounts = itr->authorized_accounts;
    auto acc_itr = std::find(accounts.begin(), accounts.end(), authorized_account);
    check(acc_itr != accounts.end(), "Account not found in authorized accounts");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.erase(acc_itr);
    });
}

ACTION abval::addactionauth (name org, name action, name authorized_account) {
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

ACTION abval::delactionauth (name org, name action, name authorized_account) {
  require_auth (org);
  actionauths_table _actionauths(get_self(), get_self().value);
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
