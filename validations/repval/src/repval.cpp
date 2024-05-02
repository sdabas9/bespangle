#include <repval.hpp>

void repval::mrrep (name org,
    name authorized,
    name rep_badge,
    string rep_badge_offchain_lookup_data,
    string rep_badge_onchain_lookup_data,
    name mutual_recognition_badge, 
    string mutual_recognition_offchain_lookup_data, 
    string mutual_recognition_onchain_lookup_data,
    uint64_t mutual_recognition_badge_rep_multiplier,
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    name sender_uniqueness_badge,
    string sender_uniqueness_badge_offchain_lookup_data, 
    string sender_uniqueness_badge_onchain_lookup_data,
    uint64_t sender_uniqueness_badge_rep_multiplier,
    name giver_rep_badge,
    string giver_rep_badge_offchain_lookup_data, 
    string giver_rep_badge_onchain_lookup_data,
    uint64_t giver_rep_badge_rep_multiplier_per_hundred_badges) {

    if (has_action_authority(org, name("mrrep"), authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");

}

ACTION repval::addactionauth (name org, name action, name authorized_account) {
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

ACTION repval::delactionauth (name org, name action, name authorized_account) {
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
