#include <aemanager.hpp>

ACTION aemanager::newemission(name authorized, name org, name emission_name, vector<asset> emitter_criteria, vector<contract_asset> emit_assets, bool cyclic) {
  require_auth(authorized);
  notify_checks_contract(org);
  require_recipient(name(ANDEMITTER_VALIDATION_CONTRACT));

  action{
    permission_level{get_self(), name("active")},
    name(ANDEMITTER_CONTRACT),
    name("newemission"),
    init_args{
      .org = org,
      .emission_name = emission_name,
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
        .badge_symbol = emitter_criteria[i].symbol,
        .notify_account = name(ANDEMITTER_CONTRACT),
        .memo = ""
      }
    }.send();
  }
}

ACTION aemanager::activate(name authorized, name org, name emission_name) {
  require_auth(authorized);
  notify_checks_contract(org);
  require_recipient(name(ANDEMITTER_VALIDATION_CONTRACT));

  action{
    permission_level{get_self(), name("active")},
    name(ANDEMITTER_CONTRACT),
    name("activate"),
    activate_args{
      .org = org,
      .emission_name = emission_name
    }
  }.send();
}

ACTION aemanager::deactivate(name authorized, name org, name emission_name) {
  require_auth(authorized);
  notify_checks_contract(org);
  require_recipient(name(ANDEMITTER_VALIDATION_CONTRACT));

  action{
    permission_level{get_self(), name("active")},
    name(ANDEMITTER_CONTRACT),
    name("deactivate"),
    deactivate_args{
      .org = org,
      .emission_name = emission_name
    }
  }.send();
}
