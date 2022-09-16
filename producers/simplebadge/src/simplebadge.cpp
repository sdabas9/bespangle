#include <simplebadge.hpp>

    // todo 
    // 1) check for cycles to throw better error message, c.
    // 2) replace values in error messages.
    // 3) put profiles contract name in a global constant
    // 4) add action to update image json.
    // 5) add action to update details json.


  ACTION simplebadge::create (name org, name badge, vector<name> parent_badges, string ipfs_image, string memo) {
    require_auth(org);
    
    badge_table _badge (_self, org.value);
    auto badge_itr = _badge.find(badge.value);

    check(badge_itr == _badge.end(), "<contractname><actionname> : <badge> already exists");
    for(auto i = 0; i < parent_badges.size(); i++) { 
      auto parentbadge_itr = _badge.require_find(parent_badges[i].value, "<parent badge> not found");
    }
    _badge.emplace(org, [&](auto& row) {
      row.badge = badge;
      row.parent_badges = parent_badges;
    });
    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT_NAME),
      name("initbadge"),
      initbadge_args {
        .org = org,
        .badge_contract = get_self(),
        .badge_name = badge,
        .ipfs_image = ipfs_image,
        .memo = memo }
    }.send();
  }


  ACTION simplebadge::issue (name org, name to, name badge, string memo ) {
    require_auth(org);
    require_recipient(to);

    badge_table _badge (_self, org.value);
    auto badge_itr = _badge.require_find(badge.value, "no simplebadge named <badge> is created");
    vector<name> all_badges;
    queue<name> _helper_queue;

    all_badges.push_back(badge);
    for(auto i = 0; i < badge_itr->parent_badges.size(); i++) {
      _helper_queue.push(badge_itr->parent_badges[i]);
    }

    while(!_helper_queue.empty()) {
      auto parent_itr = _badge.require_find(_helper_queue.front().value, "no simplebadge named <parentbadge> is created");
      all_badges.push_back(_helper_queue.front()); 
      _helper_queue.pop();
      for(auto i = 0; i < parent_itr->parent_badges.size(); i++) {
          _helper_queue.push(parent_itr->parent_badges[i]);
      }
    }

    for (auto i = 0 ; i < all_badges.size() ; i++ ) {
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT_NAME),
        name("achievement"),
        achievement_args {
          .org = org,
          .badge_contract = get_self(),
          .badge_name = all_badges[i],
          .account = to,
          .from = org,
          .count = 1,
          .memo = memo }
      }.send();
    }
    
  }

    
