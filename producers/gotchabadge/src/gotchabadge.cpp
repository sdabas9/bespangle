#include <gotchabadge.hpp>
// todos
// auth contracts
// change duration.
// change supply per duration.
// number of cycles

ACTION gotchabadge::starttime(name org, name badge, time_point_sec new_starttime) {
    check_internal_auth(name("starttime"));
    metadata_table _metadata(_self, org.value);

    // Find the record with the given badge
    auto itr = _metadata.find(badge.value);
    check(itr != _metadata.end(), "Badge not found");

    // Check if the current starttime is past the current time
    check(new_starttime >= current_time_point(), "new starttime in past");
    if (itr->starttime >= current_time_point()) {
        // Update the starttime if it's in the past
        _metadata.modify(itr, get_self(), [&](auto& row) {
            row.starttime = new_starttime;
            row.last_known_cycle_start = new_starttime;
            row.last_known_cycle_end = new_starttime + row.cycle_length - 1;
        });
    } else {
        check(false, "starttime already past");
    }
}

ACTION gotchabadge::cyclelength(name org, name badge, uint64_t new_cycle_length) {
    check_internal_auth(name("cyclelength"));
    metadata_table _metadata(_self, org.value);

    // Find the record with the given badge
    auto itr = _metadata.find(badge.value);
    check(itr != _metadata.end(), "Badge not found");

    // Update the cycle_length
    _metadata.modify(itr, get_self(), [&](auto& row) {
        row.cycle_length = new_cycle_length;
    });
}

ACTION gotchabadge::cyclesupply(name org, name badge, uint8_t new_supply_per_cycle) {
    check_internal_auth(name("cyclesupply"));
    metadata_table _metadata(_self, org.value);

    // Find the record with the given badge
    auto itr = _metadata.find(badge.value);
    check(itr != _metadata.end(), "Badge not found");

    // Update cycle supply
    _metadata.modify(itr, get_self(), [&](auto& row) {
        row.supply_per_cycle = new_supply_per_cycle;
    });
}

ACTION gotchabadge::create(name org, name badge, time_point_sec starttime, uint64_t cycle_length, uint8_t supply_per_cycle, string offchain_lookup_data, string onchain_lookup_data, string memo) {
    check_internal_auth(name("create"));
    time_point_sec current_time = time_point_sec(current_time_point());
    check(starttime >= current_time, "Gotchabadge contract - start time can not be in past");
    metadata_table _metadata(_self, org.value);

    // Todo: Add in all badges
    auto badge_itr = _metadata.find(badge.value);
    check(badge_itr == _metadata.end(), "Invalid name or <badge> with this name already exists");

    // Duration of cycle is in seconds
    _metadata.emplace(get_self(), [&](auto& row) {
        row.badge = badge;
        row.starttime = starttime;
        row.cycle_length = cycle_length;
        row.last_known_cycle_start = starttime;
        row.last_known_cycle_end = starttime + cycle_length - 1;
        row.supply_per_cycle = supply_per_cycle;
    });

    // Remote orchestrator contract call
    action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT_NAME),
        name("initbadge"),
        initbadge_args {
            .org = org,
            .badge_name = badge,
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = onchain_lookup_data,
            .memo = memo
        }
    }.send();
}


  ACTION gotchabadge::give (name org, 
    name badge, 
    name from, 
    name to, 
    uint64_t amount, 
    string memo ) {
    check_internal_auth(name("give"));

    check(from != to, "can not be same - from, to");

    metadata_table _metadata (_self, org.value);
    auto badge_itr = _metadata.require_find(badge.value, "Not a valid gotcha badge");
    time_point_sec last_known_cycle_start = badge_itr->last_known_cycle_start;
    time_point_sec last_known_cycle_end = badge_itr->last_known_cycle_end;
    uint64_t cycle_length = badge_itr->cycle_length;
    uint8_t supply_per_cycle = badge_itr->supply_per_cycle;
    time_point_sec starttime = badge_itr->starttime;
    time_point_sec current_time = time_point_sec(current_time_point());
    check(current_time >= starttime, "can not give this badge yet " + to_string(current_time.sec_since_epoch()));
    //check(false, "reached here");

    // find current cycle start and end.

    bool current_cycle_found = last_known_cycle_start <= current_time && last_known_cycle_end >= current_time;
    bool cycle_update_needed = false;

    while(!current_cycle_found) {
      cycle_update_needed = true; // optimize this.
      last_known_cycle_start = last_known_cycle_start + cycle_length;
      last_known_cycle_end = last_known_cycle_end + cycle_length;
      current_cycle_found = last_known_cycle_start <= current_time && last_known_cycle_end >= current_time;
    }
    //// update current cycle in local metadata
    if(cycle_update_needed) {
      _metadata.modify(badge_itr, get_self(), [&](auto& row) {
        row.last_known_cycle_end = last_known_cycle_end;
        row.last_known_cycle_start = last_known_cycle_start;
      }); 
    } 

    time_point_sec current_cycle_start = last_known_cycle_start;
    time_point_sec current_cycle_end = last_known_cycle_end;


    // check and update balance in local stats

    stats_table _stats (_self, org.value);
    auto account_badge_index = _stats.get_index<name("accountbadge")>();
    uint128_t account_badge_key = ((uint128_t) from.value) << 64 | badge.value;
    auto account_badge_iterator = account_badge_index.find (account_badge_key);

    if(account_badge_iterator == account_badge_index.end()) {
      check(amount <= supply_per_cycle, "<amount> exceeds available <supply_per_cycle>. Can only issue <supply_per_cycle> <badge>");
      _stats.emplace(get_self(), [&](auto& row) {
        row.id = _stats.available_primary_key();
        row.account = from;
        row.badge = badge;
        row.balance = amount;
        row.last_claimed_time = current_time;
      });
    } else if (current_cycle_start <= account_badge_iterator->last_claimed_time) {
      check(account_badge_iterator->balance + amount <= supply_per_cycle, "<balance + amount> exceeds available <supply_per_cycle>. Can only issue <supply_per_cycle - balance> <badge>");
      account_badge_index.modify(account_badge_iterator, get_self(), [&](auto& row) {
        row.balance = row.balance + amount;
        row.last_claimed_time = current_time;
      });
    } else {
      check(amount <= supply_per_cycle, "<amount> exceeds available <supply_per_cycle>. Can only issue <supply_per_cycle - balance> <badge>");
      account_badge_index.modify(account_badge_iterator, get_self(), [&](auto& row) {
        row.balance = amount;
        row.last_claimed_time = current_time;
      });
    }

    //  remote balance update and badge issuance
    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT_NAME),
      name("achievement"),
      achievement_args {
        .org = org,
        .badge = badge,
        .account = to,
        .from = from,
        .count = amount,
        .memo = memo}
    }.send();
  }

 
