#include <serval.hpp>

void serval::series (name org, 
  name authorized, 
  name series) {
    name action = name("createseries");
    action_authority_check(org, action, authorized);  
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
    action_authority_check(org, action, authorized);  
  }

void serval::latest (name org,
  name authorized, 
  name series, 
  name to, 
  uint64_t count, 
  string memo) {
    name action = name("issuelatest");
    action_authority_check(org, action, authorized);  
  }

void serval::any (name org,
  name authorized,
  name series,
  uint64_t seq_id,
  name to,
  uint64_t count,
  string memo) {
    name action = name("issueany");
    action_authority_check(org, action, authorized);  
  }

void serval::batch (name org, 
  name authorized, 
  name series, 
  vector<name> to, 
  string memo) {
    name action = name("serieslbatch");
    action_authority_check(org, action, authorized);  
  }
ACTION serval::addauth (name org, name action, name authorized_account) {
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

ACTION serval::removeauth (name org, name action, name authorized_account) {
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


