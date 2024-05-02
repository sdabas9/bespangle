#include <bamanager.hpp>

ACTION bamanager::initagg(name authorized, symbol agg_symbol, vector<symbol> badge_symbols, vector<symbol> stats_badge_symbols, string agg_description) {
    require_auth(authorized);
    string action_name = "initagg";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    for(auto i = 0 ; i < badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + badge_symbols[i].code().to_string());
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .org = org,
          .badge_symbol = badge_symbols[i],
          .notify_account = name(BOUNDED_AGG_CONTRACT),
          .memo = ""
        }
      }.send();
    }

    for(auto i = 0 ; i < stats_badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(stats_badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + stats_badge_symbols[i].code().to_string());
      auto it = std::find(badge_symbols.begin(), badge_symbols.end(), stats_badge_symbols[i]);
      check(it != badge_symbols.end(), failure_identifier + "all element in stats_badge_symbols should be present in badge_symbols");
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .org = org,
          .badge_symbol = stats_badge_symbols[i],
          .notify_account = name(BOUNDED_STATS_CONTRACT),
          .memo = ""
        }
      }.send();
    }

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("initagg"),
      initagg_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .init_badge_symbols = badge_symbols,
        .agg_description = agg_description
      }
    }.send();

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_STATS_CONTRACT),
      name("activate"),
      stats_activate_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .badge_symbols = stats_badge_symbols
      }
    }.send();
}

ACTION bamanager::addinitbadge(name authorized, symbol agg_symbol, vector<symbol> badge_symbols) {
    require_auth(authorized);
    string action_name = "addinitbadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    for(auto i = 0 ; i < badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + badge_symbols[i].code().to_string());
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .org = org,
          .badge_symbol = badge_symbols[i],
          .notify_account = name(BOUNDED_AGG_CONTRACT),
          .memo = ""
        }
      }.send();
    }

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("addinitbadge"),
      addinitbadge_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .badge_symbols = badge_symbols
      }
    }.send();

}
ACTION bamanager::reminitbadge(name authorized, symbol agg_symbol, vector<symbol> badge_symbols) {
    require_auth(authorized);
    string action_name = "reminitbadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    for(auto i = 0 ; i < badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + badge_symbols[i].code().to_string());
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .org = org,
          .badge_symbol = badge_symbols[i],
          .notify_account = name(BOUNDED_AGG_CONTRACT),
          .memo = ""
        }
      }.send();
    }

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("addinitbadge"),
      addinitbadge_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .badge_symbols = badge_symbols
      }
    }.send();
}
ACTION bamanager::addstatbadge(name authorized, symbol agg_symbol, vector<symbol> badge_symbols) {
    require_auth(authorized);
    string action_name = "addstatbadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    for(auto i = 0 ; i < badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + badge_symbols[i].code().to_string());
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .org = org,
          .badge_symbol = badge_symbols[i],
          .notify_account = name(BOUNDED_AGG_CONTRACT),
          .memo = ""
        }
      }.send();
      
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .org = org,
          .badge_symbol = badge_symbols[i],
          .notify_account = name(BOUNDED_STATS_CONTRACT),
          .memo = ""
        }
      }.send();
    }

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_STATS_CONTRACT),
      name("activate"),
      stats_activate_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .badge_symbols = badge_symbols
      }
    }.send();

}
ACTION bamanager::remstatbadge(name authorized, symbol agg_symbol, vector<symbol> badge_symbols) {
    require_auth(authorized);
    string action_name = "remstatbadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));
    for(auto i = 0 ; i < badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + badge_symbols[i].code().to_string());
    }

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_STATS_CONTRACT),
      name("deactivate"),
      stats_deactivate_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .badge_symbols = badge_symbols
      }
    }.send();
}

ACTION bamanager::initseq(name authorized, symbol agg_symbol, string sequence_description) {
    require_auth(authorized);
    string action_name = "initseq";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("initseq"),
      initseq_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .sequence_description = sequence_description
      }
    }.send();
}

ACTION bamanager::actseq(name authorized, symbol agg_symbol, vector<uint64_t> seq_ids) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "actseq";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);
    notify_checks_contract(org);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("actseq"),
      actseq_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = seq_ids
      }
    }.send();
   
}

ACTION bamanager::actseqai(name authorized, symbol agg_symbol) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "actseqai";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
        
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("actseqai"),
      actseqai_args {
        .org = org,
        .agg_symbol = agg_symbol
      }
    }.send();

}

ACTION bamanager::actseqfi(name authorized, symbol agg_symbol) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "actseqfi";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("actseqfi"),
      actseqfi_args {
        .org = org,
        .agg_symbol = agg_symbol
      }
    }.send();

}

ACTION bamanager::endseq(name authorized, symbol agg_symbol, vector<uint64_t> seq_ids) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "endseq";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("endseq"),
      endseq_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = seq_ids
      }
    }.send();
}

ACTION bamanager::endseqaa(name authorized, symbol agg_symbol) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "endseqaa";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("endseqaa"),
      endseqaa_args {
        .org = org,
        .agg_symbol = agg_symbol
      }
    }.send();
}

ACTION bamanager::endseqfa(name authorized, symbol agg_symbol) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "endseqfa";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);
    
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("endseqfa"),
      actseq_args {
        .org = org,
        .agg_symbol = agg_symbol
      }
    }.send();
}

ACTION bamanager::addbadge(name authorized, symbol agg_symbol, vector<uint64_t> seq_ids, vector<symbol> badge_symbols) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "addbadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    for(auto i = 0 ; i < badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + badge_symbols[i].code().to_string());
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("addfeature"),
        addfeature_args{
          .org = org,
          .badge_symbol = badge_symbols[i],
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
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_ids = seq_ids,
        .badge_symbols = badge_symbols
      }
    }.send();
}

ACTION bamanager::pauseall(name authorized, symbol agg_symbol, uint64_t seq_id) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pauseall";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pauseall"),
      pauseall_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_id = seq_id
      }
    }.send();

}
ACTION bamanager::pausebadge(name authorized, symbol agg_symbol, uint64_t badge_agg_seq_id) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pausebadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pausebadge"),
      pausebadge_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .badge_agg_seq_id = badge_agg_seq_id
      }
    }.send();
}
ACTION bamanager::pausebadges(name authorized, symbol agg_symbol, uint64_t seq_id, vector<symbol> badge_symbols) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pausebadges";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    for(auto i = 0 ; i < badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + badge_symbols[i].code().to_string());
    }

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pausebadges"),
      pausebadges_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_id = seq_id,
        .badge_symbols = badge_symbols
      }
    }.send();
}
ACTION bamanager::pauseallfa(name authorized, symbol agg_symbol) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "pauseallfa";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("pauseallfa"),
      pauseallfa_args {
        .org = org,
        .agg_symbol = agg_symbol
      }
    }.send();
}
ACTION bamanager::resumeall(name authorized, symbol agg_symbol, uint64_t seq_id) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "resumeall";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";

    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("resumeall"),
      resumeall_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_id = seq_id
      }
    }.send();
}
ACTION bamanager::resumebadge(name authorized, symbol agg_symbol, uint64_t badge_agg_seq_id) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "resumebadge";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("resumebadge"),
      resumebadge_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .badge_agg_seq_id = badge_agg_seq_id
      }
    }.send();
}
ACTION bamanager::resumebadges(name authorized, symbol agg_symbol, uint64_t seq_id, vector<symbol> badge_symbols) {
    require_auth(authorized);
    require_recipient(name(BOUNDED_AGG_VALIDATION_CONTRACT));

    string action_name = "resumebadges";
    string failure_identifier = "CONTRACT: bamanager, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_agg_symbol(agg_symbol, failure_identifier);

    notify_checks_contract(org);

    for(auto i = 0 ; i < badge_symbols.size(); i++) {
      check(org == get_org_from_badge_symbol(badge_symbols[i], failure_identifier), failure_identifier + "Org mismatch for badge " + badge_symbols[i].code().to_string());
    }
    action {
      permission_level{get_self(), name("active")},
      name(BOUNDED_AGG_CONTRACT),
      name("resumebadges"),
      resumebadges_args {
        .org = org,
        .agg_symbol = agg_symbol,
        .seq_id = seq_id,
        .badge_symbols = badge_symbols
      }
    }.send();
}
