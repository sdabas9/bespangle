#include <mrval.hpp>

void mrval::initgotcha (name org,
      name authorized, 
      name badge, 
      time_point_sec starttime, 
      uint64_t cycle_length, 
      uint8_t supply_per_cycle, 
      string offchain_lookup_data, 
      string onchain_lookup_data,
      vector<name> consumers,
      string memo) {

    if (has_action_authority(org, name("initgotcha"), authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");

}

void mrval::changestart(name org, name authorized, name badge, time_point_sec new_starttime) {
    if (has_action_authority(org, name("changestart"), authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");
}

void mrval::changelength(name org, name authorized, name badge, uint64_t new_cycle_length) {
    if (has_action_authority(org, name("changelength"), authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");
}

void mrval::changesupply(name org, name authorized, name badge, uint8_t new_supply_per_cycle) {
    if (has_action_authority(org, name("changesupply"), authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");
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