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

ACTION metadata::initbadge (name org, name badge_contract, name badge_name, string ipfs_image, string memo) {
  check_authorization(org, badge_contract);
  init(org, badge_contract, badge_name, ipfs_image);

}

ACTION metadata::addfeature (name org, name badge_contract, name badge_name, name notify_account, string memo) {
  require_auth(org);
  
  badge_table _badge( _self, org.value );
  auto contract_badge_index = _badge.get_index<name("contractbadge")>();
  uint128_t contract_badge_key = ((uint128_t) badge_contract.value) << 64 | badge_name.value;
  auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);
  check(contract_badge_iterator != contract_badge_index.end(), "<contractname>,<action name> : <org> <contract> <badge> not found");
  
  vector<name> new_notify_accounts ;
  for( auto i = 0; i < contract_badge_iterator->notify_accounts.size(); i++) {
    check(notify_account != contract_badge_iterator->notify_accounts[i], "<thiscontractname>,<action name> : <contract> <badge> is already ");
    new_notify_accounts.push_back(contract_badge_iterator->notify_accounts[i]);
  }
  new_notify_accounts.push_back (notify_account);
  contract_badge_index.modify(contract_badge_iterator, get_self(), [&](auto& row){
    row.notify_accounts = new_notify_accounts;
  });

  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("addnotify"),
    downstream_notify_args {
      .org = org,
      .badge_contract = contract_badge_iterator->badge_contract,
      .badge_name = contract_badge_iterator->badge_name,
      .notify_account = notify_account,
      .memo = memo,
      .badge_id = contract_badge_iterator->badge_id,
      .ipfs_hashes = contract_badge_iterator->ipfs_hashes,
      .rarity_counts = contract_badge_iterator->rarity_counts
      }
  }.send();

}

ACTION metadata::addnotify(
  name org,
  name badge_contract,
  name badge_name,
  name notify_account,
  string memo, 
  uint64_t badge_id, 
  vector<ipfs_hash> ipfs_hashes,
  uint32_t rarity_counts) {
    require_auth(get_self());
    require_recipient (notify_account); 
}

ACTION metadata::delfeature (name org, name badge_contract, name badge_name, name notify_account, string memo) {
  require_auth(org);
  
  badge_table _badge( _self, org.value );
  auto contract_badge_index = _badge.get_index<name("contractbadge")>();
  uint128_t contract_badge_key = ((uint128_t) badge_contract.value) << 64 | badge_name.value;
  auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);
  check(contract_badge_iterator != contract_badge_index.end(), "<contractname>,<action name> : <org> <contract> <badge> not found");

  vector<name> new_notify_accounts ;
  for( auto i = 0; i < contract_badge_iterator->notify_accounts.size(); i++) {
    if(notify_account != contract_badge_iterator->notify_accounts[i]) {
      new_notify_accounts.push_back(contract_badge_iterator->notify_accounts[i]);
    }       
  }
  contract_badge_index.modify(contract_badge_iterator, get_self(), [&](auto& row){
    row.notify_accounts = new_notify_accounts;
  });
  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("delnotify"),
    downstream_notify_args {
      .org = org,
      .badge_contract = contract_badge_iterator->badge_contract,
      .badge_name = contract_badge_iterator->badge_name,
      .notify_account = notify_account,
      .memo = memo,
      .badge_id = contract_badge_iterator->badge_id,
      .ipfs_hashes = contract_badge_iterator->ipfs_hashes,
      .rarity_counts = contract_badge_iterator->rarity_counts
      }
  }.send();
}

ACTION metadata::delnotify( 
  name org,
  name badge_contract,
  name badge_name,
  name notify_account,
  string memo, 
  uint64_t badge_id, 
  vector<ipfs_hash> ipfs_hashes,
  uint32_t rarity_counts) {
    require_auth(get_self());
    require_recipient (notify_account); 
}

ACTION metadata::achievement (name org, name badge_contract, name badge_name, name account, name from, uint8_t count, string memo) {
  check_authorization (org, badge_contract);
  check_account_prefs (org, account);
  
  badge_table _badge( _self, org.value );
  auto contract_badge_index = _badge.get_index<name("contractbadge")>();
  uint128_t contract_badge_key = ((uint128_t) badge_contract.value) << 64 | badge_name.value;
  auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);

  check(contract_badge_iterator != contract_badge_index.end(), "<contractname>,<action name> : <org> <contract> <badge> not found");
 
  contract_badge_index.modify(contract_badge_iterator, get_self(), [&](auto& row){
    row.rarity_counts = row.rarity_counts + count;
  });
   
  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("notifyachiev"),
    notifyachievement_args {
      .org = org,
      .badge_contract = badge_contract,
      .badge_name = badge_name,
      .account = account,
      .from = from,
      .count = count,
      .memo = memo,
      .badge_id = contract_badge_iterator->badge_id,
      .notify_accounts = contract_badge_iterator->notify_accounts}
  }.send();

  //deduct_platform_fees (org);

}

ACTION metadata::notifyachiev (name org, 
  name badge_contract, 
  name badge_name,
  name account, 
  name from,
  uint8_t count,
  string memo,
  uint64_t badge_id,  
  vector<name> notify_accounts) {
  
  require_auth(get_self());
  for(auto i = 0; i < notify_accounts.size(); i++) {
    require_recipient (notify_accounts[i]);
  }
}
