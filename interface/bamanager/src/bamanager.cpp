#include <bamanager.hpp>

ACTION bamanager::activeseq(name authorized, name org, name agg, vector<uint64_t> seq_ids) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "activeseq";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("activeseq"),
      activeseq_args {
        .agg_symbol = agg_symbol,
        .seq_ids = seq_ids
      }
    }.send();

}
ACTION bamanager::activeseqli(name authorized, name org, name agg) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "activeseqli";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("activeseqli"),
      activeseqli_args {
        .agg_symbol = agg_symbol
      }
    }.send();
}
ACTION bamanager::activeseqlp(name authorized, name org, name agg) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "activeseqlp";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("activeseqlp"),
      activeseqlp_args {
        .agg_symbol = agg_symbol
        }
    }.send();
}
ACTION bamanager::initagg(name authorized, name org, name agg, string description) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "initagg";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("initagg"),
      initagg_args {
        .agg_symbol = agg_symbol,
        .description = description
      }
    }.send();
}

ACTION bamanager::initseq(name authorized, name org, name agg, string description) {

    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "initseq";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("initseq"),
      initseq_args {
        .agg_symbol = agg_symbol,
        .description = description
      }
    }.send();

}
ACTION bamanager::pauseseq(name authorized, name org, name agg, vector<uint64_t> seq_ids) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pauseseq";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pauseseq"),
      pauseseq_args {
        .agg_symbol = agg_symbol,
        .seq_ids = seq_ids
      }
    }.send();
}
ACTION bamanager::endseq(name authorized, name org, name agg, vector<uint64_t> seq_ids) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "endseq";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("endseq"),
      endseq_args {
        .agg_symbol = agg_symbol,
        .seq_ids = seq_ids
      }
    }.send();
}

ACTION bamanager::addbadgeli(name authorized, name org, name agg, vector<name> badges) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "addbadgeli";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    vector<symbol> badge_symbols;
    for (auto i = 0 ; i < badges.size() ; i++) {
      symbol badge_symbol = validate_and_get_symbol(org, badges[i], failure_identifier);
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .badge_symbol = badge_symbol,
          .notify_account = name(BOUNDED_AGG_CONTRACT),
          .memo = ""
        }
      }.send();
    }
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("addbadgeli"),
      addbadgeli_args {
        .agg_symbol = agg_symbol,
        .badge_symbols = badge_symbols
      }
    }.send();
}
ACTION bamanager::addbadge(name authorized, name org, name agg, vector<uint64_t> seq_ids, vector<name> badges) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "addbadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    vector<symbol> badge_symbols;
    for (auto i = 0 ; i < badges.size() ; i++) {
      symbol badge_symbol = validate_and_get_symbol(org, badges[i], failure_identifier);
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .badge_symbol = badge_symbol,
          .notify_account = name(BOUNDED_AGG_CONTRACT),
          .memo = ""
        }
      }.send();
    }

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("addbadge"),
      addbadge_args {
        .agg_symbol = agg_symbol,
        .seq_ids = seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send();
}
