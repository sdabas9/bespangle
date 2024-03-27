#include <baval.hpp>

void baval::activeseq(name authorized, name org, name agg, vector<uint64_t> seq_ids) {
    string action_name = "activeseq";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void baval::activeseqli(name authorized, name org, name agg) {
    string action_name = "activeseqli";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void baval::activeseqlp(name authorized, name org, name agg) {
    string action_name = "activeseqlp";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void baval::initagg(name authorized, name org, name agg, string description) {
    string action_name = "initagg";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void baval::initseq(name authorized, name org, name agg, string description) {
    string action_name = "initseq";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void baval::pauseseq(name authorized, name org, name agg, vector<uint64_t> seq_ids) {
    string action_name = "pauseseq";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void baval::endseq(name authorized, name org, name agg, vector<uint64_t> seq_ids) {
    string action_name = "endseq";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void baval::addbadgeli(name authorized, name org, name agg, vector<name> badges) {
    string action_name = "addbadgeli";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}

void baval::addbadge(name authorized, name org, name agg, vector<uint64_t> seq_ids, vector<name> badges) {
    string action_name = "addbadge";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");
}


ACTION baval::addaggauth(name org, name action, name agg, name authorized_account) {
    require_auth (org);

    aggauths_table aggauths(get_self(), org.value);
    auto secondary_index = aggauths.get_index<"byactionagg"_n>();
    uint128_t secondary_key = aggauths::combine_names(action, agg);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->agg == agg, "agg with specified action and agg not found");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.push_back(authorized_account);
    });
}

ACTION baval::delaggauth(name org, name action, name agg, name authorized_account) {
    require_auth (org);

    string action_name = "delaggauth";
    string failure_identifier = "CONTRACT: baval, ACTION: " + action_name + ", MESSAGE: ";
 
    aggauths_table aggauths(get_self(), org.value);
    auto secondary_index = aggauths.get_index<"byactionagg"_n>();
    uint128_t secondary_key = aggauths::combine_names(action, agg);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->agg == agg, failure_identifier + "agg with specified action and agg not found");

    auto& accounts = itr->authorized_accounts;
    auto acc_itr = std::find(accounts.begin(), accounts.end(), authorized_account);
    check(acc_itr != accounts.end(), failure_identifier + "Account not found in authorized accounts");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.erase(acc_itr);
    });
}

ACTION baval::addactionauth (name org, name action, name authorized_account) {
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

ACTION baval::delactionauth (name org, name action, name authorized_account) {
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
