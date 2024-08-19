#include <aemanager.hpp>

ACTION aemanager::newemission(name authorized, 
  symbol emission_symbol, 
  vector<asset> emitter_criteria, 
  vector<asset> emit_badges, 
  bool cyclic) {
  require_auth(authorized);
  string action_name = "newemission";
  string failure_identifier = "CONTRACT: aemanager, ACTION: " + action_name + ", MESSAGE: ";
  name org = get_org_from_internal_symbol(emission_symbol, failure_identifier);

  if(org!=authorized) {
    require_recipient(name(ANDEMITTER_VALIDATION_CONTRACT));
  }

  vector<contract_asset> emit_assets;

  for(asset a : emitter_criteria) {
    check(org == get_org_from_internal_symbol(emission_symbol, failure_identifier), "org not same for all assets in emitter_criteria");
  }
  for(asset a : emit_badges) {
    check(org == get_org_from_internal_symbol(emission_symbol, failure_identifier), "org not same for all assets in emit_badges");
  }
  notify_checks_contract(org);
  for(auto i = 0 ; i < emit_badges.size(); i++) {
    emit_assets.push_back (contract_asset {
      .contract = name(SIMPLEBADGE_CONTRACT),
      .emit_asset = emit_badges[i]
    });
  }

  action{
    permission_level{get_self(), name("active")},
    name(ANDEMITTER_CONTRACT),
    name("newemission"),
    init_args{
      .org = org,
      .emission_symbol = emission_symbol,
      .emitter_criteria = emitter_criteria,
      .emit_assets = emit_assets,
      .cyclic = cyclic
    }
  }.send();

  for (auto i = 0; i < emitter_criteria.size(); i++) {
    action{
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      addfeature_args{
        .org = org,
        .badge_symbol = emitter_criteria[i].symbol,
        .notify_account = name(ANDEMITTER_CONTRACT),
        .memo = ""
      }
    }.send();
  }
}

ACTION aemanager::activate(name authorized, symbol emission_symbol) {
  require_auth(authorized);
  string action_name = "activate";
  string failure_identifier = "CONTRACT: aemanager, ACTION: " + action_name + ", MESSAGE: ";  
  name org = get_org_from_internal_symbol(emission_symbol, failure_identifier);
  if(org!=authorized) {
    require_recipient(name(ANDEMITTER_VALIDATION_CONTRACT));
  }
  notify_checks_contract(org);

  action{
    permission_level{get_self(), name("active")},
    name(ANDEMITTER_CONTRACT),
    name("activate"),
    activate_args{
      .org = org,
      .emission_symbol = emission_symbol
    }
  }.send();
}

ACTION aemanager::deactivate(name authorized, symbol emission_symbol) {
  require_auth(authorized);
  string action_name = "deactivate";
  string failure_identifier = "CONTRACT: aemanager, ACTION: " + action_name + ", MESSAGE: ";
  name org = get_org_from_internal_symbol(emission_symbol, failure_identifier);
  if(org!=authorized) {
    require_recipient(name(ANDEMITTER_VALIDATION_CONTRACT));
  }
  notify_checks_contract(org);
  action{
    permission_level{get_self(), name("active")},
    name(ANDEMITTER_CONTRACT),
    name("deactivate"),
    deactivate_args{
      .org = org,
      .emission_symbol = emission_symbol
    }
  }.send();
}
