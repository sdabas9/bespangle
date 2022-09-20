# metadata

This contract 
- notifies badge creation and issuance to subscribed downstream consumers.
- stores ipfs hashes related to a badge.
- maintains counts of badges issued.
- maintains org and badge_contract subscription. this helps in authorization.

## ACTIONS

### action for registering trusted_badge_contract
Org calls this action to register badge contracts like simplebadge and gotchabadge.
ACTION recognize (name org, name trusted_badge_contract);

### action for registering a badge
This action is called by trusted_badge_contract when a new badge is created.
ACTION initbadge (name org, 
    name badge_contract, 
    name badge_name, 
    string ipfs_image, 
    string memo);

  
### add a feature to a badge.
currently supported features are aabadge, cumulative and rounds.
- adding aabadge feature enables writing to atomic assets as NTT.
- adding cumulative feature enables writing to cumulative smart contracts, which maintains cumulative counts of badges issued to an account.
- adding rounds feature enables writing to rounds smart contracts and all subfeatures associated with it.
  ACTION addfeature (name org, 
    name badge_contract,
    name badge_name,
    name notify_account,
    string memo);

### notification to downstream on addfeature action
  ACTION addnotify(
    name org,
    name badge_contract,
    name badge_name,
    name notify_accounts,
    string memo, 
    uint64_t badge_id, 
    vector<ipfs_hash> ipfs_hashes,
    uint32_t rarity_counts); 

### delete a feature from a badge
Call this action to remove an added feature.
  ACTION delfeature (name org, 
    name badge_contract,
    name badge_name,
    name notify_account,
    string memo);

### notification to downstream on delfeature action
  ACTION delnotify(
    name org,
    name badge_contract,
    name badge_name,
    name notify_accounts,
    string memo, 
    uint64_t badge_id, 
    vector<ipfs_hash> ipfs_hashes,
    uint32_t rarity_counts); 

### receive a badge.
This action is called by trusted_badge_contract when a badge is issued.
  ACTION achievement (name org,
    name badge_contract,
    name badge_name,
    name account,
    name from,
    uint8_t count,
    string memo);

### notification to subsribed downstream on achievement action
  ACTION notifyachiev (name org, 
    name badge_contract, 
    name badge_name,
    name account, 
    name from,
    uint8_t count,
    string memo,
    uint64_t badge_id,  
    vector<name> notify_accounts);
