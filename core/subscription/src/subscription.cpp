#include <subscription.hpp>

void subscription::notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts
) {
}

void subscription::buyservice (name from, name to, asset amount, string memo) {}

ACTION subscription::newpack(name package,
      uint64_t unique_issuance_size,
      uint64_t expiry_duration,
      uint8_t active_seasons,
      asset ram_cost_without_volatility) {
      }

ACTION subscription::disablepack(name package) {
}

ACTION subscription::enablepack(name package) {
}

  // inserts record in mcost table
ACTION subscription::membercost(asset first_member_cost) {

}

  // inserts record in mduetime table
ACTION subscription::graceperiod(uint64_t number_of_grace_seconds_for_admin_payment) {

}

ACTION subscription::genbill(name org) {
}


