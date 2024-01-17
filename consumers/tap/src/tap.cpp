#include <tap.hpp>

  void tap::notifyinit(
    name org,
    name badge,
    name notify_account,
    string memo,
    string offchain_lookup_data,
    string onchain_lookup_data,
    uint64_t rarity_counts) {

    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.find(badge.value);
    if (itr == _tapstatus.end()) {
      _tapstatus.emplace(get_self(), [&](auto& row){
        row.badge = badge;
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

  void tap::notifyachiev (name org, 
      name badge,
      name account, 
      name from,
      uint64_t count,
      string memo,
      vector<name> notify_accounts) {

    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge.value, "tap contract: Somethings wrong, should not have reached here");
    
    check(itr->pause == false, "Distribution paused");
    
    if(itr->supply_bound) {
      check(itr->current_supply + count <= itr->supplycap, "Exceeded max supply");
      _tapstatus.modify(itr, get_self(), [&](auto& row){
        row.current_supply = row.current_supply + count;
      });
    } 
    
    if(itr->time_bound) {
      time_point_sec current_time = time_point_sec(current_time_point());
      check(current_time >= itr->start_time, "Distribution not started");
      check(current_time < itr->end_time, "Distribution ended");
    }




    // check if paused
    // check if out of time range
    // check if out of supply range
  }

  ACTION tap::pause(name org, name assetname) {
    require_auth(org);
    uint64_t badge_id = assetname.value;
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.pause = true;
    });
  }


  ACTION tap::resume(name org, name assetname) {
    require_auth(org);
    uint64_t badge_id = assetname.value;
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.pause = false;
    });
  }

  ACTION tap::timebound(name org, name assetname, time_point_sec start_time, time_point_sec end_time) {
    require_auth(org);
    uint64_t badge_id = assetname.value;
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.time_bound = true;
      row.start_time = start_time;
      row.end_time = end_time;
    });
  }

  ACTION tap::removetb(name org, name assetname) {
    require_auth(org);
    uint64_t badge_id = assetname.value;
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.time_bound = false;
    });
  }

  ACTION tap::supplybound(name org, name assetname, uint64_t supplycap) {
    require_auth(org);
    uint64_t badge_id = assetname.value;
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.supply_bound = true;
      row.supplycap = supplycap;
    });
  }

  ACTION tap::removesb(name org, name assetname) {
    require_auth(org);
    uint64_t badge_id = assetname.value;
    tapstatus_table _tapstatus( _self, org.value );
    auto itr = _tapstatus.require_find(badge_id, "<tap> consumer is not set for <issuing_contract><assetname>");
    
    _tapstatus.modify(itr, get_self(), [&](auto& row){
      row.supply_bound = false;
    });
  }