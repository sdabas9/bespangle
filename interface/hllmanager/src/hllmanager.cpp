#include <hllmanager.hpp>

ACTION hllmanager::newemission(
    name authorized,
    symbol badge_symbol,
    vector<symbol> sender_uniqueness_badge_symbols) {

  require_auth(authorized);
  string action_name = "newemission";
  string failure_identifier = "CONTRACT: hllmanager, ACTION: " + action_name + ", MESSAGE: ";

  name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);
  if(org != authorized) {
    require_recipient(name(HLL_EMITTER_VALIDATION_CONTRACT));
  }
  
  for(symbol s : sender_uniqueness_badge_symbols) {
    check(org == get_org_from_internal_symbol(s, failure_identifier), "org not same for all assets in sender_uniqueness_badge_symbols");
  }
  notify_checks_contract(org);

  action{
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args{
      .org = org,
      .badge_symbol = badge_symbol,
      .notify_account = name(HLL_EMITTER_CONTRACT),
      .memo = ""
    }
  }.send();

  action{
    permission_level{get_self(), name("active")},
    name(HLL_EMITTER_CONTRACT),
    name("newemission"),
    newemission_args{
      .org = org,
      .badge_symbol = badge_symbol,
      .sender_uniqueness_badge_symbols = sender_uniqueness_badge_symbols
    }
  }.send();

}

ACTION hllmanager::activate(name authorized, symbol badge_symbol) {
  require_auth(authorized);
  string action_name = "activate";
  string failure_identifier = "CONTRACT: hllmanager, ACTION: " + action_name + ", MESSAGE: ";  

  name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);
  if(org != authorized) {
    require_recipient(name(HLL_EMITTER_VALIDATION_CONTRACT));
  }
  notify_checks_contract(org);

  action{
    permission_level{get_self(), name("active")},
    name(HLL_EMITTER_CONTRACT),
    name("activate"),
    activate_args{
      .org = org,
      .badge_symbol = badge_symbol
    }
  }.send();
}

ACTION hllmanager::deactivate(name authorized, symbol badge_symbol) {
  require_auth(authorized);
  string action_name = "deactivate";
  string failure_identifier = "CONTRACT: hllmanager, ACTION: " + action_name + ", MESSAGE: ";
  name org = get_org_from_internal_symbol(badge_symbol, failure_identifier);
  if(org != authorized) {
    require_recipient(name(HLL_EMITTER_VALIDATION_CONTRACT));
  }
  notify_checks_contract(org);
  action{
    permission_level{get_self(), name("active")},
    name(HLL_EMITTER_CONTRACT),
    name("deactivate"),
    deactivate_args{
      .org = org,
      .badge_symbol = badge_symbol
    }
  }.send();
}
