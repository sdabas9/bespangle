#include <claimasset.hpp>

    // todo 
    // 1) check for cycles to throw better error message, c.
    // 2) replace values in error messages.
    // 3) put profiles contract name in a global constant
    // 4) add action to update image json.
    // 5) add action to update details json.

  void claimasset::ncreate (name org, 
    name assetname, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo) {
  
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("create"),
      create_args {
        .org = org,
        .assetname = assetname,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = onchain_lookup_data,
        .memo = memo }
    }.send();  
  }

  void claimasset::naddclaimer (name org, name account, name assetname, uint64_t account_cap, string memo) {
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("addclaimer"),
      addclaimer_args {
        .org = org,
        .account = account,
        .assetname = assetname,
        .account_cap = account_cap,
        .memo = memo }
    }.send(); 
  }

  void claimasset::nclaim(name org, 
      name to, 
      name assetname, 
      string memo ) {
    action {
      permission_level{get_self(), name("active")},
      name(get_self()),
      name("claim"),
      claim_args {
        .org = org,
        .to = to,
        .assetname = assetname,
        .memo = memo }
    }.send(); 
  }
  
  ACTION claimasset::create (name org, 
    name assetname,
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo) {
    
    require_auth(get_self());

    metadata_table _metadata (_self, org.value);
    auto metadata_itr = _metadata.find(assetname.value);
    check(metadata_itr == _metadata.end(), "<contractname><actionname> : <assetname> already exists");

    _metadata.emplace(get_self(), [&](auto& row) {
      row.assetname = assetname;
    });

    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT_NAME),
      name("initbadge"),
      initbadge_args {
        .org = org,
        .badge_contract = get_self(),
        .badge_name = assetname,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = onchain_lookup_data,
        .memo = memo }
    }.send();
  }
 

  ACTION claimasset::claim (name org, name account, name assetname, string memo ) {
    require_auth(get_self());
    metadata_table _metadata (_self, org.value);
    auto metadata_itr = _metadata.require_find(assetname.value, "no asset named <assetname> is created");
    
    claimlist_table _claimlist (get_self(), org.value);
      
    auto account_asset_index = _claimlist.get_index<name("accountasset")>();
    uint128_t account_asset_key = ((uint128_t) account.value) << 64 | assetname.value;
    auto account_asset_iterator = account_asset_index.find (account_asset_key);

    check(account_asset_iterator != account_asset_index.end(), "<assetname> not availble for claim for <account>");
    check(account_asset_iterator->account_cap > account_asset_iterator->claimed_amount, "<account> has reached claim limit for <assetname>");

    account_asset_index.modify(account_asset_iterator, get_self(), [&](auto& row) {
      row.claimed_amount = row.claimed_amount + 1;
    });

    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT_NAME),
      name("achievement"),
      achievement_args {
        .org = org,
        .badge_contract = get_self(),
        .badge_name = assetname,
        .account = account,
        .from = org,
        .count = 1,
        .memo = memo }
    }.send();
  }

  ACTION claimasset::addclaimer (name org, name account, name assetname, uint64_t account_cap, string memo) {
    require_auth(get_self());
    
    claimlist_table _claimlist (get_self(), org.value);
      
    auto account_asset_index = _claimlist.get_index<name("accountasset")>();
    uint128_t account_asset_key = ((uint128_t) account.value) << 64 | assetname.value;
    auto account_asset_iterator = account_asset_index.find (account_asset_key);
    
    if(account_asset_iterator == account_asset_index.end()) {
      _claimlist.emplace(get_self(), [&](auto& row) {
        row.id = _claimlist.available_primary_key();
        row.account = account;
        row.assetname = assetname;
        row.account_cap = account_cap;
      });
    } else {
      account_asset_index.modify(account_asset_iterator, get_self(), [&](auto& row) {
        row.account_cap = row.account_cap + account_cap;
      });
    }
  }


    
