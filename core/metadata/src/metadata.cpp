#include <metadata.hpp>
#include <json.hpp>
using json = nlohmann::json;

/*ACTION metadata::recognize (name trusted_contract) {
  require_auth (get_self());
  authorized_contracts_table _authorized_contracts( _self, _self.value );
  auto itr = _authorized_contracts.find(trusted_contract.value);
  check(itr == _authorized_contracts.end(), "<trusted_contract> already authorized to issues badges");
  _authorized_contracts.emplace(get_self(), [&](auto& row){
    row.trusted_contract = trusted_contract;
  });
}*/

/*ACTION metadata::isrecognized (name org, name contract) {
  authorized_contracts_table _authorized_contracts( _self, org.value );
  auto itr = _authorized_contracts.find(contract.value);
  check(itr != _authorized_contracts.end(), "<contract> is not recognized for <org>");
}*/

ACTION metadata::initbadge (name org, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
  check_internal_auth(name("initbadge"));
  init(org, badge, offchain_lookup_data, onchain_lookup_data, memo);

}

ACTION metadata::mergeinfo (name org, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
  check_internal_auth(name("mergeinfo"));
  badge_table _badge( _self, org.value );
  auto badge_iterator = _badge.find (badge.value);
  check(badge_iterator != _badge.end(), "<action name> : <org> <contract> <badge> not found");

  
  if(!offchain_lookup_data.empty()) {
    check(json::accept(offchain_lookup_data), "offchain_lookup_data is not a valid json"); 
    string existing_offchain_data = badge_iterator->offchain_lookup_data;
    check(json::accept(existing_offchain_data), "existing offchain_lookup_data is not a valid json"); 
    json existing_offchain_lookup_json = json::parse(existing_offchain_data);
    json new_offchain_lookup_json = json::parse(offchain_lookup_data);
    existing_offchain_lookup_json.merge_patch(new_offchain_lookup_json);
    _badge.modify(badge_iterator, get_self(), [&](auto& row){
      row.offchain_lookup_data = existing_offchain_lookup_json.dump();
    });
  }

  
  if(!onchain_lookup_data.empty()) {
    check(json::accept(onchain_lookup_data), "onchain_lookup_data is not a valid json"); 
    string existing_onchain_data = badge_iterator->onchain_lookup_data;
    check(json::accept(existing_onchain_data), "existing onchain_lookup_data is not a valid json"); 
    json existing_onchain_lookup_json = json::parse(existing_onchain_data);
    json new_onchain_lookup_json = json::parse(onchain_lookup_data);
    existing_onchain_lookup_json.merge_patch(new_onchain_lookup_json);
    _badge.modify(badge_iterator, get_self(), [&](auto& row){
      row.onchain_lookup_data = existing_onchain_lookup_json.dump();
    });
    
  }


}

ACTION metadata::addfeature (name org, name badge, name notify_account, string memo) {
  check_internal_auth(name("addfeature"));  
  badge_table _badge( _self, org.value );
  auto badge_iterator = _badge.find (badge.value);
  check(badge_iterator != _badge.end(), "addfeature action, metadata contract : <org> <contract> <badge> not found");
  
  vector<name> new_notify_accounts ;
  for( auto i = 0; i < badge_iterator->notify_accounts.size(); i++) {
    if(notify_account == badge_iterator->notify_accounts[i]) {
      return;
    }
    new_notify_accounts.push_back(badge_iterator->notify_accounts[i]);
  }
  new_notify_accounts.push_back (notify_account);
  _badge.modify(badge_iterator, get_self(), [&](auto& row){
    row.notify_accounts = new_notify_accounts;
  });
  // image
  // display name
  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("addnotify"),
    downstream_notify_args {
      .org = org,
      .badge = badge_iterator->badge,
      .notify_account = notify_account,
      .memo = memo,
      .offchain_lookup_data = badge_iterator->offchain_lookup_data,
      .onchain_lookup_data = badge_iterator->onchain_lookup_data,
      .rarity_counts = badge_iterator->rarity_counts
      }
  }.send();

}

ACTION metadata::addnotify(
  name org,
  name badge,
  name notify_account,
  string memo,
  string offchain_lookup_data,
  string onchain_lookup_data,
  uint64_t rarity_counts) {
    require_auth(get_self());
    require_recipient (notify_account); 
}

ACTION metadata::delfeature (name org, name badge, name notify_account, string memo) {
  check_internal_auth(name("delfeature"));  
  badge_table _badge( _self, org.value );
  auto badge_iterator = _badge.find (badge.value);
  check(badge_iterator != _badge.end() , "<action name> : <org> <contract> <badge> not found");

  vector<name> new_notify_accounts ;
  for( auto i = 0; i < badge_iterator->notify_accounts.size(); i++) {
    if(notify_account != badge_iterator->notify_accounts[i]) {
      new_notify_accounts.push_back(badge_iterator->notify_accounts[i]);
    }       
  }
  _badge.modify(badge_iterator, get_self(), [&](auto& row){
    row.notify_accounts = new_notify_accounts;
  });
  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("delnotify"),
    downstream_notify_args {
      .org = org,
      .badge = badge_iterator->badge,
      .notify_account = notify_account,
      .memo = memo,
      .offchain_lookup_data = badge_iterator->offchain_lookup_data,
      .onchain_lookup_data = badge_iterator->onchain_lookup_data,
      .rarity_counts = badge_iterator->rarity_counts
      }
  }.send();
}

ACTION metadata::delnotify( 
  name org,
  name badge,
  name notify_account,
  string memo, 
  string offchain_lookup_data,
  string onchain_lookup_data,
  uint64_t rarity_counts) {
    require_auth(get_self());
    require_recipient (notify_account); 
}

ACTION metadata::achievement (name org, name badge, name account, name from, uint64_t count, string memo) {
  check_internal_auth(name("achievement"));
  //check_account_prefs (org, account);
  badge_table _badge( _self, org.value );
  auto badge_iterator = _badge.find (badge.value);

  check(badge_iterator != _badge.end() , "<contractname>,<action name> : <org> <contract> <badge> not found");
 
  _badge.modify(badge_iterator, get_self(), [&](auto& row){
    row.rarity_counts = row.rarity_counts + count;
  });
   
  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("notifyachiev"),
    notifyachievement_args {
      .org = org,
      .badge = badge,
      .account = account,
      .from = from,
      .count = count,
      .memo = memo,
      .notify_accounts = badge_iterator->notify_accounts}
  }.send();

  //deduct_platform_fees (org);

}

ACTION metadata::notifyachiev (name org, 
  name badge,
  name account, 
  name from,
  uint64_t count,
  string memo,
  vector<name> notify_accounts) {
  
  require_auth(get_self());
  for(auto i = 0; i < notify_accounts.size(); i++) {
    require_recipient (notify_accounts[i]);
  }
}
