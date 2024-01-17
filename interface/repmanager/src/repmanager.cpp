#include <repmanager.hpp>

  ACTION repmanager::mrrep (name org,
    name authorized,
    name rep_badge,
    string rep_badge_offchain_lookup_data,
    string rep_badge_onchain_lookup_data,
    name mutual_recognition_badge, 
    string mutual_recognition_offchain_lookup_data, 
    string mutual_recognition_onchain_lookup_data,
    uint64_t mutual_recognition_badge_rep_multiplier,
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    name sender_uniqueness_badge,
    string sender_uniqueness_badge_offchain_lookup_data, 
    string sender_uniqueness_badge_onchain_lookup_data,
    uint64_t sender_uniqueness_badge_rep_multiplier,
    name giver_rep_badge,
    string giver_rep_badge_offchain_lookup_data, 
    string giver_rep_badge_onchain_lookup_data,
    uint64_t giver_rep_badge_rep_multiplier_per_hundred_badges) {
    require_auth(authorized);

    notify_checks_contract(org);
    require_recipient(name(REPUTATION_MANAGER_VALIDATION_CONTRACT));

    std::vector<name> empty_name_vector;
    action {
      permission_level{get_self(), name("active")},
      name(SIMPLEBADGE_CONTRACT),
      name("create"),
      createsimple_args {
        .org = org,
        .badge = rep_badge,
        .parent_badges = empty_name_vector,
        .offchain_lookup_data = rep_badge_offchain_lookup_data,
        .onchain_lookup_data = rep_badge_onchain_lookup_data,
        .memo = ""}
    }.send();

    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      addfeature_args {
        .org = org,
        .badge_name = rep_badge,
        .notify_account = name(CUMULATIVE_CONTRACT),
        .memo = ""}
    }.send();

    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      addfeature_args {
        .org = org,
        .badge_name = rep_badge,
        .notify_account = name(STATISTICS_CONTRACT),
        .memo = ""}
    }.send();

    action {
      permission_level{get_self(), name("active")},
      name(GOTCHABADGE_CONTRACT),
      name("create"),
      creategotcha_args {
        .org = org,
        .badge = mutual_recognition_badge,
        .starttime = starttime,
        .cycle_length = cycle_length,
        .supply_per_cycle = supply_per_cycle,
        .offchain_lookup_data = mutual_recognition_offchain_lookup_data,
        .onchain_lookup_data = mutual_recognition_onchain_lookup_data,
        .memo = "" }
    }.send();



    action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = mutual_recognition_badge,
      .notify_account = name(CUMULATIVE_CONTRACT),
      .memo = ""}
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = mutual_recognition_badge,
      .notify_account = name(HLL_EMITTER_CONTRACT),
      .memo = ""}
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = mutual_recognition_badge,
      .notify_account = name(AND_EMITTER_CONTRACT),
      .memo = ""}
    }.send();

    std::map<name, uint64_t> gotcha_badge_emitter_criteria;
    gotcha_badge_emitter_criteria[mutual_recognition_badge] = 1;
    std::map<repmanager::asset_contract_name, uint64_t> gotcha_badge_emit_assets;
    repmanager::asset_contract_name asset_contract = {.issuing_contract = name(SIMPLEBADGE_CONTRACT) ,
     .asset_name = rep_badge};
    gotcha_badge_emit_assets[asset_contract] = mutual_recognition_badge_rep_multiplier; 

    action {
      permission_level{get_self(), name("active")},
      name(AND_EMITTER_CONTRACT),
      name("newemission"),
      andemitter_newemission_args {
        .org = org,
        .emission_name = mutual_recognition_badge,
        .emitter_criteria = gotcha_badge_emitter_criteria,
        .emit_assets = gotcha_badge_emit_assets,
        .cyclic = true
        }
    }.send();

    action {
      permission_level{get_self(), name("active")},
      name(AND_EMITTER_CONTRACT),
      name("activate"),
      andemitter_activate_args {
        .org = org,
        .emission_name = mutual_recognition_badge
        }
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = mutual_recognition_badge,
      .notify_account = name(GIVERREP_CONTRACT),
      .memo = ""}
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(GIVERREP_CONTRACT),
    name("newemission"),
    giverrep_newemission_args {
      .org = org,
      .badge = mutual_recognition_badge,
      .source_account_rep_badge = rep_badge,
      .emit_badge = giver_rep_badge }
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("create"),
    createsimple_args {
      .org = org,
      .badge = sender_uniqueness_badge,
      .parent_badges = empty_name_vector,
      .offchain_lookup_data = sender_uniqueness_badge_offchain_lookup_data,
      .onchain_lookup_data = sender_uniqueness_badge_onchain_lookup_data,
      .memo = ""}
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = sender_uniqueness_badge,
      .notify_account = name(CUMULATIVE_CONTRACT),
      .memo = ""}
    }.send();

    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("addfeature"),
      addfeature_args {
        .org = org,
        .badge_name = sender_uniqueness_badge,
        .notify_account = name(AND_EMITTER_CONTRACT),
        .memo = ""}
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(HLL_EMITTER_CONTRACT),
    name("newemission"),
    hllemitter_newemission_args {
      .org = org,
      .badge = mutual_recognition_badge,
      .sender_uniqueness_badge = sender_uniqueness_badge}
    }.send();

    std::map<name, uint64_t> sender_uniqueness_badge_emitter_criteria;
    sender_uniqueness_badge_emitter_criteria[sender_uniqueness_badge] = 1;
    std::map<repmanager::asset_contract_name, uint64_t> sender_uniqueness_badge_emit_assets;
    sender_uniqueness_badge_emit_assets[asset_contract] = sender_uniqueness_badge_rep_multiplier; 

    action {
      permission_level{get_self(), name("active")},
      name(AND_EMITTER_CONTRACT),
      name("newemission"),
      andemitter_newemission_args {
        .org = org,
        .emission_name = sender_uniqueness_badge,
        .emitter_criteria = sender_uniqueness_badge_emitter_criteria,
        .emit_assets = sender_uniqueness_badge_emit_assets,
        .cyclic = true
        }
    }.send();

    action {
      permission_level{get_self(), name("active")},
      name(AND_EMITTER_CONTRACT),
      name("activate"),
      andemitter_activate_args {
        .org = org,
        .emission_name = sender_uniqueness_badge
        }
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("create"),
    createsimple_args {
      .org = org,
      .badge = giver_rep_badge,
      .parent_badges = empty_name_vector,
      .offchain_lookup_data = giver_rep_badge_offchain_lookup_data,
      .onchain_lookup_data = giver_rep_badge_onchain_lookup_data,
      .memo = ""}
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = giver_rep_badge,
      .notify_account = name(CUMULATIVE_CONTRACT),
      .memo = ""}
    }.send();

    action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT),
    name("addfeature"),
    addfeature_args {
      .org = org,
      .badge_name = giver_rep_badge,
      .notify_account = name(AND_EMITTER_CONTRACT),
      .memo = ""}
    }.send();

    std::map<name, uint64_t> giver_rep_badge_emitter_criteria;
    giver_rep_badge_emitter_criteria[giver_rep_badge] = 100;
    std::map<repmanager::asset_contract_name, uint64_t> giver_rep_badge_emit_assets;
    giver_rep_badge_emit_assets[asset_contract] = giver_rep_badge_rep_multiplier_per_hundred_badges; 

    action {
      permission_level{get_self(), name("active")},
      name(AND_EMITTER_CONTRACT),
      name("newemission"),
      andemitter_newemission_args {
        .org = org,
        .emission_name = giver_rep_badge,
        .emitter_criteria = giver_rep_badge_emitter_criteria,
        .emit_assets = giver_rep_badge_emit_assets,
        .cyclic = true
        }
    }.send();

    action {
      permission_level{get_self(), name("active")},
      name(AND_EMITTER_CONTRACT),
      name("activate"),
      andemitter_activate_args {
        .org = org,
        .emission_name = giver_rep_badge
        }
    }.send();

  }

