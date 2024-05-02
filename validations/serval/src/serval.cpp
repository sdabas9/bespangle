#include <serval.hpp>

void serval::series (name org, 
  name authorized, 
  name series) {
    name action = name("createseries");
    if (has_action_authority(org, action, authorized)) {
        return;
    }
    if (has_series_authority(org, action, series, authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");
  }

void serval::next (name org, 
  name authorized, 
  name series, 
  name badge, 
  string offchain_lookup_data, 
  string onchain_lookup_data, 
  vector<name> consumers,
  string memo) {
    name action = name("createnext");
    if (has_action_authority(org, action, authorized)) {
        return;
    }
    if (has_series_authority(org, action, series, authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");
  }

void serval::latest (name org,
  name authorized, 
  name series, 
  name to, 
  uint64_t count, 
  string memo) {
    name action = name("issuelatest");
    if (has_action_authority(org, action, authorized)) {
        return;
    }
    if (has_series_authority(org, action, series, authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");
  }

void serval::any (name org,
  name authorized,
  name series,
  uint64_t seq_id,
  name to,
  uint64_t count,
  string memo) {
    name action = name("issueany");
    if (has_action_authority(org, action, authorized)) {
        return;
    }
    if (has_series_authority(org, action, series, authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");
  }

void serval::batch (name org, 
  name authorized, 
  name series, 
  vector<name> to, 
  string memo) {
    name action = name("serieslbatch");
    if (has_action_authority(org, action, authorized)) {
        return;
    }
    if (has_series_authority(org, action, series, authorized)) {
        return;
    }
    check(false, "Unauthorized account to execute action");
  }

ACTION serval::addseriesauth(name org, name action, name series, name authorized_account) {
    require_auth (org);

    seriesauths_table seriesauths(get_self(), org.value);
    auto secondary_index = seriesauths.get_index<"actionseries"_n>();
    uint128_t secondary_key = seriesauths::combine_names(action, series);
    auto itr = secondary_index.find(secondary_key);
    if(itr == secondary_index.end() || itr->action != action || itr->series != series) {
      vector<name> authorized_accounts;
      authorized_accounts.push_back(authorized_account);
      seriesauths.emplace(get_self(), [&](auto& row) {
        row.id = seriesauths.available_primary_key();
        row.action = action;
        row.series = series;
        row.authorized_accounts = authorized_accounts;
      });
    } else {
      secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.push_back(authorized_account);
      });
    }
}

ACTION serval::delseriesauth(name org, name action, name series, name authorized_account) {
    require_auth (org);

    seriesauths_table seriesauths(get_self(), org.value);
    auto secondary_index = seriesauths.get_index<"actionseries"_n>();
    uint128_t secondary_key = seriesauths::combine_names(action, series);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->series == series, "Authority for series and action not found");

    auto& accounts = itr->authorized_accounts;
    auto acc_itr = std::find(accounts.begin(), accounts.end(), authorized_account);
    check(acc_itr != accounts.end(), "Account not found in authorized accounts");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.erase(acc_itr);
    });
}

ACTION serval::addactionauth (name org, name action, name authorized_account) {
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

ACTION serval::delactionauth (name org, name action, name authorized_account) {
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


