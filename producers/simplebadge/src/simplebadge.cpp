#include <simplebadge.hpp>

    // todo 
    // 1) check for cycles to throw better error message, c.
    // 2) replace values in error messages.
    // 3) put profiles contract name in a global constant
    // 4) add action to update image json.
    // 5) add action to update details json.

  ACTION simplebadge::create (
      name org,
      symbol badge_symbol, 
      string offchain_lookup_data, 
      string onchain_lookup_data, 
      string memo) {
    string action_name = "create";
    string failure_identifier = "CONTRACT: simplebadge, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);   
    
    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("initbadge"),
      initbadge_args {
        .org = org,
        .badge_symbol = badge_symbol,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = onchain_lookup_data,
        .memo = memo }
    }.send();
  }

  ACTION simplebadge::issue (name org, asset badge_asset, name to, string memo) {
    string action_name = "issue";
    string failure_identifier = "CONTRACT: simplebadge, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);   
    
    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("achievement"),
      achievement_args {
        .org = org,
        .badge_asset = badge_asset,
        .from = get_self(),
        .to = to,
        .memo = memo }
    }.send();
  }

    
