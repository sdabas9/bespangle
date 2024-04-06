#include <aeval.hpp>

void aeval::newemission(name authorized, 
    name org, 
    name emission_name, 
    vector<asset> emitter_criteria, 
    vector<contract_asset> emit_assets, 
    bool cyclic) {

    string action_name = "newemission";
    string failure_identifier = "CONTRACT: aeval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_emission_authority(org, name(action_name), emission_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");

}
    
void aeval::activate(name authorized, 
    name org, 
    name emission_name) {

    string action_name = "activate";
    string failure_identifier = "CONTRACT: aeval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_emission_authority(org, name(action_name), emission_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");

}

void aeval::deactivate(name authorized, 
    name org, 
    name emission_name) {

    string action_name = "deactivate";
    string failure_identifier = "CONTRACT: aeval, ACTION: " + action_name + ", MESSAGE: ";

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_emission_authority(org, name(action_name), emission_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");    
}


ACTION aeval::addemissauth(name org, name action, name emission, name authorized_account) {
    require_auth (org);

    emissauths_table emissauths(get_self(), org.value);
    auto secondary_index = emissauths.get_index<"actionemiss"_n>();
    uint128_t secondary_key = emissauths::combine_names(action, emission);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->emission == emission, "Emission with specified action and badge not found");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.push_back(authorized_account);
    });
}

ACTION aeval::delemissauth(name org, name action, name emission, name authorized_account) {
    require_auth (org);

    string action_name = "delemissauth";
    string failure_identifier = "CONTRACT: aeval, ACTION: " + action_name + ", MESSAGE: ";
 
    emissauths_table emissauths(get_self(), org.value);
    auto secondary_index = emissauths.get_index<"actionemiss"_n>();
    uint128_t secondary_key = emissauths::combine_names(action, emission);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->emission == emission, failure_identifier + "Emission with specified action and badge not found");

    auto& accounts = itr->authorized_accounts;
    auto acc_itr = std::find(accounts.begin(), accounts.end(), authorized_account);
    check(acc_itr != accounts.end(), failure_identifier + "Account not found in authorized accounts");

    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.erase(acc_itr);
    });
}

ACTION aeval::addactionauth (name org, name action, name authorized_account) {
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

ACTION aeval::delactionauth (name org, name action, name authorized_account) {
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