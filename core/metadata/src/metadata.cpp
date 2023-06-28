#include <metadata.hpp>

ACTION metadata::recognize (name org, name trusted_contract) {
  require_auth (org);
  authorized_contracts_table _authorized_contracts( _self, org.value );
  auto itr = _authorized_contracts.find(trusted_contract.value);
  check(itr == _authorized_contracts.end(), "<trusted_contract> already authorized to issues badges");
  _authorized_contracts.emplace(get_self(), [&](auto& row){
    row.trusted_contract = trusted_contract;
  });
}

ACTION metadata::isrecognized (name org, name contract) {
  authorized_contracts_table _authorized_contracts( _self, org.value );
  auto itr = _authorized_contracts.find(contract.value);
  check(itr != _authorized_contracts.end(), "<contract> is not recognized for <org>");
}

ACTION metadata::initbadge (name org, name badge_name, string offchain_lookup_data, string onchain_lookup_data, string memo) {
  check_authorization(org);
  init(org, badge_name, offchain_lookup_data, onchain_lookup_data, memo);

}

void metadata::naddfeatur (name org, 
  name badge_name,
  name notify_account,
  string memo) {

  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("addfeature"),
    local_addfeature_args {
      .org = org,
      .badge_name = badge_name,
      .notify_account = notify_account,
      .memo = memo
      }
  }.send();
}

ACTION metadata::addfeature (name org, name badge_name, name notify_account, string memo) {
  require_auth(get_self());
  
  badge_table _badge( _self, org.value );
  auto badge_iterator = _badge.find (badge_name.value);
  check(badge_iterator != _badge.end(), "<action name> : <org> <contract> <badge> not found");
  
  vector<name> new_notify_accounts ;
  for( auto i = 0; i < badge_iterator->notify_accounts.size(); i++) {
    check(notify_account != badge_iterator->notify_accounts[i], "<thiscontractname>,<action name> : <contract> <badge> is already ");
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
      .badge_name = badge_iterator->badge_name,
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
  name badge_name,
  name notify_account,
  string memo,
  string offchain_lookup_data,
  string onchain_lookup_data,
  uint64_t rarity_counts) {
    require_auth(get_self());
    require_recipient (notify_account); 
}

ACTION metadata::delfeature (name org, name badge_name, name notify_account, string memo) {
  require_auth(org);
  
  badge_table _badge( _self, org.value );
  auto badge_iterator = _badge.find (badge_name.value);
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
      .badge_name = badge_iterator->badge_name,
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
  name badge_name,
  name notify_account,
  string memo, 
  string offchain_lookup_data,
  string onchain_lookup_data,
  uint64_t rarity_counts) {
    require_auth(get_self());
    require_recipient (notify_account); 
}

ACTION metadata::achievement (name org, name badge_name, name account, name from, uint64_t count, string memo) {
  check_authorization (org);
  //check_account_prefs (org, account);
  
  badge_table _badge( _self, org.value );
  auto badge_iterator = _badge.find (badge_name.value);

  check(badge_iterator != _badge.end(), "<contractname>,<action name> : <org> <contract> <badge> not found");
 
  _badge.modify(badge_iterator, get_self(), [&](auto& row){
    row.rarity_counts = row.rarity_counts + count;
  });
   
  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("notifyachiev"),
    notifyachievement_args {
      .org = org,
      .badge_name = badge_name,
      .account = account,
      .from = from,
      .count = count,
      .memo = memo,
      .notify_accounts = badge_iterator->notify_accounts}
  }.send();

  //deduct_platform_fees (org);

}

ACTION metadata::notifyachiev (name org, 
  name badge_name,
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
