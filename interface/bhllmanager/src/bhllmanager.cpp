#include <bhllmanager.hpp>

ACTION bhllmanager::newemission(name authorized,
    symbol agg_symbol,
    symbol badge_symbol,
    vector<symbol> sender_uniqueness_badge_symbols) {

  require_auth(authorized);
  string action_name = "newemission";
  string failure_identifier = "CONTRACT: bhllmanager, ACTION: " + action_name + ", MESSAGE: ";
  require_recipient(name(BOUNDED_HLL_VALIDATION_CONTRACT));

  name org = get_org_from_internal_symbol(agg_symbol, failure_identifier);
  check(org == get_org_from_internal_symbol(badge_symbol, failure_identifier), "badge_symbol and agg_symbol org not same");

  for(symbol s : sender_uniqueness_badge_symbols) {
    check(org == get_org_from_internal_symbol(s, failure_identifier), "org not same for agg and a symbol in sender_uniqueness_badge_symbols");
  }
  notify_checks_contract(org);

  action{
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args{
      .org = org,
      .badge_symbol = badge_symbol,
      .notify_account = name(BOUNDED_HLL_CONTRACT),
      .memo = ""
    }
  }.send();

  action{
    permission_level{get_self(), name("active")},
    name(BOUNDED_HLL_CONTRACT),
    name("newemission"),
    newemission_args{
      .org = org,
      .agg_symbol = agg_symbol,
      .badge_symbol = badge_symbol,
      .sender_uniqueness_badge_symbols = sender_uniqueness_badge_symbols
    }
  }.send();

}

ACTION bhllmanager::activate(name authorized, symbol agg_symbol, symbol badge_symbol) {
  require_auth(authorized);
  string action_name = "activate";
  string failure_identifier = "CONTRACT: bhllmanager, ACTION: " + action_name + ", MESSAGE: ";  
  require_recipient(name(BOUNDED_HLL_VALIDATION_CONTRACT));
  name org = get_org_from_internal_symbol(agg_symbol, failure_identifier);
  check(org == get_org_from_internal_symbol(agg_symbol, failure_identifier), "org not same for agg_symbol and badge_symbol");
  notify_checks_contract(org);

  action{
    permission_level{get_self(), name("active")},
    name(BOUNDED_HLL_CONTRACT),
    name("activate"),
    activate_args{
      .org = org,
      .agg_symbol = agg_symbol,
      .badge_symbol = badge_symbol
    }
  }.send();
}

ACTION bhllmanager::deactivate(name authorized, symbol agg_symbol, symbol badge_symbol) {
  require_auth(authorized);
  string action_name = "deactivate";
  string failure_identifier = "CONTRACT: bhllmanager, ACTION: " + action_name + ", MESSAGE: ";
  require_recipient(name(BOUNDED_HLL_VALIDATION_CONTRACT));
  name org = get_org_from_internal_symbol(agg_symbol, failure_identifier);
  check(org == get_org_from_internal_symbol(agg_symbol, failure_identifier), "org not same for agg_symbol and badge_symbol");
  notify_checks_contract(org);
  action{
    permission_level{get_self(), name("active")},
    name(BOUNDED_HLL_CONTRACT),
    name("deactivate"),
    deactivate_args{
      .org = org,
      .agg_symbol = agg_symbol,
      .badge_symbol = badge_symbol
    }
  }.send();
}
