#include <bamanager.hpp>

ACTION bamanager::initagg(name authorized, name org, name agg, string first_seq_description) {
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
        .first_seq_description = first_seq_description
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

ACTION bamanager::actseq(name authorized, name org, name agg, vector<uint64_t> seq_ids, vector<name> badges) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "actseq";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    vector<symbol> badge_symbols;
    for (auto i = 0 ; i < badges.size() ; i++) {
      symbol badge_symbol = validate_and_get_symbol(org, badges[i], failure_identifier);
      badge_symbols.push_back(badge_symbol);
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

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("actseq"),
      actseq_args {
        .agg_symbol = agg_symbol,
        .seq_ids = seq_ids
      }
    }.send();
   
}

ACTION bamanager::actseqai(name authorized, name org, name agg, vector<name> badges) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "actseqai";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    vector<symbol> badge_symbols;
    for (auto i = 0 ; i < badges.size() ; i++) {
      symbol badge_symbol = validate_and_get_symbol(org, badges[i], failure_identifier);
      badge_symbols.push_back(badge_symbol);
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
      name("addbadgeai"),
      addbadgeai_args {
        .agg_symbol = agg_symbol,
        .badge_symbols = badge_symbols
      }
    }.send(); 
        
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("actseqai"),
      actseqai_args {
        .agg_symbol = agg_symbol
      }
    }.send();

}

ACTION bamanager::actseqfi(name authorized, name org, name agg, vector<name> badges) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "actseqfi";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    vector<symbol> badge_symbols;
    for (auto i = 0 ; i < badges.size() ; i++) {
      symbol badge_symbol = validate_and_get_symbol(org, badges[i], failure_identifier);
      badge_symbols.push_back(badge_symbol);
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
      name("addbadgefi"),
      addbadgefi_args {
        .agg_symbol = agg_symbol,
        .badge_symbols = badge_symbols
      }
    }.send(); 

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("actseqfi"),
      actseqfi_args {
        .agg_symbol = agg_symbol
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

ACTION bamanager::endseqaa(name authorized, name org, name agg) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "endseqaa";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("endseqaa"),
      endseqaa_args {
        .agg_symbol = agg_symbol
      }
    }.send();
}

ACTION bamanager::endseqfa(name authorized, name org, name agg) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "endseqfa";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("endseqfa"),
      actseq_args {
        .agg_symbol = agg_symbol
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
      badge_symbols.push_back(badge_symbol);
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

ACTION bamanager::pauseall(name authorized, name org, name agg, uint64_t seq_id) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pauseall";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pauseall"),
      pauseall_args {
        .agg_symbol = agg_symbol,
        .seq_id = seq_id
      }
    }.send();

}
ACTION bamanager::pausebadge(name authorized, name org, name agg, uint64_t badge_agg_seq_id) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pausebadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pausebadge"),
      pausebadge_args {
        .agg_symbol = agg_symbol,
        .badge_agg_seq_id = badge_agg_seq_id
      }
    }.send();
}
ACTION bamanager::pausebadges(name authorized, name org, name agg, uint64_t seq_id, vector<name> badges) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pausebadges";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    vector<symbol> badge_symbols;
    for (auto i = 0 ; i < badges.size() ; i++) {
      symbol badge_symbol = validate_and_get_symbol(org, badges[i], failure_identifier);
      badge_symbols.push_back(badge_symbol);
    }

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pausebadges"),
      pausebadges_args {
        .agg_symbol = agg_symbol,
        .seq_id = seq_id,
        .badge_symbols = badge_symbols
      }
    }.send();
}
ACTION bamanager::pauseallfa(name authorized, name org, name agg) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pauseallfa";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pauseallfa"),
      pauseallfa_args {
        .agg_symbol = agg_symbol
      }
    }.send();
}
ACTION bamanager::resumeall(name authorized, name org, name agg, uint64_t seq_id) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "resumeall";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("resumeall"),
      resumeall_args {
        .agg_symbol = agg_symbol,
        .seq_id = seq_id
      }
    }.send();
}
ACTION bamanager::resumebadge(name authorized, name org, name agg, uint64_t badge_agg_seq_id) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "resumebadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("resumebadge"),
      resumebadge_args {
        .agg_symbol = agg_symbol,
        .badge_agg_seq_id = badge_agg_seq_id
      }
    }.send();
}
ACTION bamanager::resumebadges(name authorized, name org, name agg, uint64_t seq_id, vector<name> badges) {
    require_auth(authorized);
    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "resumebadges";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    symbol agg_symbol = validate_and_get_agg_symbol(org, agg, failure_identifier);
    vector<symbol> badge_symbols;
    for (auto i = 0 ; i < badges.size() ; i++) {
      symbol badge_symbol = validate_and_get_symbol(org, badges[i], failure_identifier);
      badge_symbols.push_back(badge_symbol);
    }
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("resumebadges"),
      resumebadges_args {
        .agg_symbol = agg_symbol,
        .seq_id = seq_id,
        .badge_symbols = badge_symbols
      }
    }.send();
}
