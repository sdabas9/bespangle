#include <tap.hpp>

  void tap::notifyinit(
    name org,
    name badge_contract,
    name badge_name,
    name notify_account,
    string memo, 
    uint64_t badge_id, 
    string offchain_lookup_data,
    string onchain_lookup_data,
    uint32_t rarity_counts) {

    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.find(badge_id);
    if (itr == _tapstatus.end()) {
      _tapstatus.emplace(get_self(), [&](auto& row){
        row.badge_id = badge_id;
        row.pause = false;
        row.time_bound = false;
        row.supply_bound = false;
        row.current_supply = rarity_counts;
      });
    } else {
      _tapstatus.modify(itr, get_self(), [&](auto& row){
        row.current_supply = rarity_counts;
      });
    }
  }

  void tap::notifyachiev (
    name org, 
    name badge_contract, 
    name badge_name,
    name account, 
    name from,
    uint8_t count,
    string memo,
    uint64_t badge_id,  
    vector<name> notify_accounts) {

    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "Somethings wrong, should not have reached here");
    
    check(itr->pause == false, "Distribution paused");
    
    if(itr->supply_bound) {
      check(itr->current_supply + count <= itr->supplycap, "Exceeded max supply");
    } 
    
    if(itr->time_bound) {
      time_point_sec current_time = time_point_sec(current_time_point());
      check(current_time >= itr->start_time, "Distribution not started");
      check(current_time < itr->end_time, "Distribution ended");
    }

    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.current_supply = row.current_supply + count;
    });


    // check if paused
    // check if out of time range
    // check if out of supply range
  }

  ACTION tap::pause(name org, name issuing_contract, name assetname) {
    require_auth(org);
    uint64_t badge_id = get_badge_id(org, issuing_contract, assetname);
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.pause = true;
    });
  }


  ACTION tap::resume(name org, name issuing_contract, name assetname) {
    require_auth(org);
    uint64_t badge_id = get_badge_id(org, issuing_contract, assetname);
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.pause = false;
    });
  }

  ACTION tap::timebound(name org, name issuing_contract, name assetname, time_point_sec start_time, time_point_sec end_time) {
    require_auth(org);
    uint64_t badge_id = get_badge_id(org, issuing_contract, assetname);
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.time_bound = true;
      row.start_time = start_time;
      row.end_time = end_time;
    });
  }

  ACTION tap::removetb(name org, name issuing_contract, name assetname) {
    require_auth(org);
    uint64_t badge_id = get_badge_id(org, issuing_contract, assetname);
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.time_bound = false;
    });
  }

  ACTION tap::supplybound(name org, name issuing_contract, name assetname, uint64_t supplycap) {
    require_auth(org);
    uint64_t badge_id = get_badge_id(org, issuing_contract, assetname);
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.supply_bound = true;
      row.supplycap = supplycap;
    });
  }

  ACTION tap::removesb(name org, name issuing_contract, name assetname) {
    require_auth(org);
    uint64_t badge_id = get_badge_id(org, issuing_contract, assetname);
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.supply_bound = false;
    });
  }