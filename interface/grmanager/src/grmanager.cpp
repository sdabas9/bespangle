#include <grmanager.hpp>

ACTION grmanager::newemission(name authorized,
    symbol badge_symbol,
    symbol giver_rep_badge_symbol, 
    vector<symbol> emit_badge_symbols) {

  require_auth(authorized);
  string action_name = "newemission";
  string failure_identifier = "CONTRACT: grmanager, ACTION: " + action_name + ", MESSAGE: ";
  require_recipient(name(GIVER_REP_VALIDATION_CONTRACT));

  name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);
  check(org == get_org_from_internal_symbol(giver_rep_badge_symbol, failure_identifier), "badge_symbol and giver_rep_badge_symbol org not same");

  for(symbol s : emit_badge_symbols) {
    check(org == get_org_from_internal_symbol(s, failure_identifier), "org not same for all assets in emit_badge_symbols");
  }
  notify_checks_contract(org);

  action{
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args{
      .org = org,
      .badge_symbol = badge_symbol,
      .notify_account = name(GIVER_REP_CONTRACT),
      .memo = ""
    }
  }.send();

  action{
    permission_level{get_self(), name("active")},
    name(GIVER_REP_CONTRACT),
    name("newemission"),
    newemission_args{
      .org = org,
      .badge_symbol = badge_symbol,
      .giver_rep_badge_symbol = giver_rep_badge_symbol,
      .emit_badge_symbols = emit_badge_symbols
    }
  }.send();

}

ACTION grmanager::activate(name authorized, symbol badge_symbol) {
  require_auth(authorized);
  string action_name = "activate";
  string failure_identifier = "CONTRACT: grmanager, ACTION: " + action_name + ", MESSAGE: ";  
  require_recipient(name(GIVER_REP_VALIDATION_CONTRACT));
  name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);
  notify_checks_contract(org);

  action{
    permission_level{get_self(), name("active")},
    name(GIVER_REP_CONTRACT),
    name("activate"),
    activate_args{
      .org = org,
      .badge_symbol = badge_symbol
    }
  }.send();
}

ACTION grmanager::deactivate(name authorized, symbol badge_symbol) {
  require_auth(authorized);
  string action_name = "deactivate";
  string failure_identifier = "CONTRACT: grmanager, ACTION: " + action_name + ", MESSAGE: ";
  require_recipient(name(GIVER_REP_VALIDATION_CONTRACT));
  name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);
  notify_checks_contract(org);
  action{
    permission_level{get_self(), name("active")},
    name(GIVER_REP_CONTRACT),
    name("deactivate"),
    deactivate_args{
      .org = org,
      .badge_symbol = badge_symbol
    }
  }.send();
}
