#include <org.hpp>

ACTION org::chkscontract(name org, name checks_contract) {
  require_auth(org);
  checks_table _checks(get_self(), get_self().value);
  auto itr = _checks.find(org.value);
  if (itr == _checks.end()) {
    _checks.emplace(org, [&](auto& row) {
      row.org = org;
      row.checks_contract = checks_contract;
    });
  } else {
    _checks.modify(itr, org, [&](auto& row) {
      row.checks_contract = checks_contract;
    });
  }
}

ACTION org::initorgcode(name org, string org_code) {
  require_auth(org); // Authentication by the org

  // Validate org_code length and content
  check(org_code.size() == 4, "org_code must be exactly 4 characters long");
  check(std::all_of(org_code.begin(), org_code.end(), [](char c) { return std::isalpha(c) && std::islower(c); }), "org_code must only contain lowercase alphabets");

  // Convert string org_code to eosio::name after validation
  eosio::name converted_org_code = eosio::name(org_code);

  orgcode_index orgcodes(get_self(), get_self().value);

  // Ensure org_code is unique
  auto org_code_index = orgcodes.get_index<"orgcodeidx"_n>();
  auto org_code_iterator = org_code_index.find(converted_org_code.value);
  check(org_code_iterator == org_code_index.end() || org_code_iterator->org_code!=converted_org_code, "org_code must be unique");

  // Check if the org already exists
  auto iterator = orgcodes.find(org.value);
  check(iterator == orgcodes.end(), "Organization already exists");

  // Insert new orgcode
  orgcodes.emplace(org, [&](auto& row) {
    row.org = org;
    row.org_code = converted_org_code;
  });

  action {
    permission_level{get_self(), name("active")},
    name(SUBSCRIPTION_CONTRACT),
    name("haspackage"),
    haspackage_args {
      .org = org
    }
  }.send();


}

ACTION initautocode(name org) {
    require_auth(org);
    badgecode_table _badgecodes(get_self(), get_self().value);
    emissioncode_table _emissioncodes(get_self(), get_self().value);

    auto it = _badgecodes.find(org.value);
    check(it == _badgecodes.end(), "Badge code already initialized for this organization");

    _badgecodes.emplace(get_self(), [&](auto& row) {
        row.org = org;
        row.last_badge_code = "aaz"; // Initial badge code
    });

    auto emit_itr = _emissioncodes.find(org.value);
    check(it == _emissioncodes.end(), "Emission code already initialized for this organization");

    _emissioncodes.emplace(get_self(), [&](auto& row) {
        row.org = org;
        row.last_emission_code = "aaz"; // Initial emission code
    });

    auto bounty_itr = _bountycodes.find(org.value);
    check(it == _bountycodes.end(), "Bounty code already initialized for this organization");

    _bountycodes.emplace(get_self(), [&](auto& row) {
        row.org = org;
        row.last_bounty_code = "aaz"; // Initial bounty code
    });
}

// Action to compute and update the next badge code
    // Action to compute and update the next badge symbol
ACTION nextbadge(name org) {
    string action_name = "nextbadge";
    string failure_identifier = "CONTRACT: org, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    badgecode_table _badgecodes(get_self(), get_self().value);
    orgcode_table _orgcodes(get_self(), get_self().value);

    auto org_it = _orgcodes.find(org.value);
    check(org_it != _orgcodes.end(), "Organization code not found for this organization");

    auto it = _badgecodes.find(org.value);
    check(it != _badgecodes.end(), "Badge code not initialized for this organization");

    string org_code = org_it->org_code.to_string();
    string current_code = it->last_badge_symbol.code().to_string();

    // Ensure we can increment the badge code
    check(current_code != "zzz", "Cannot increment badge code beyond 'zzz'");

    string next_code = increment_code(current_code);
    string next_symbol_str = org_code + next_code;

    symbol next_symbol(symbol_code(next_symbol_str), 0);

    _badgecodes.modify(it, get_self(), [&](auto& row) {
        row.last_badge_symbol = next_symbol;
    });
}

ACTION nextemission(name org) {
    string action_name = "nextemission";
    string failure_identifier = "CONTRACT: org, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    emissioncode_table _emissioncodes(get_self(), get_self().value);
    orgcode_table _orgcodes(get_self(), get_self().value);

    auto org_it = _orgcodes.find(org.value);
    check(org_it != _orgcodes.end(), "Organization code not found for this organization");

    auto it = _emissioncodes.find(org.value);
    check(it != _emissioncodes.end(), "Emission code not initialized for this organization");

    string org_code = org_it->org_code.to_string();
    string current_code = it->last_emission_symbol.code().to_string();

    // Ensure we can increment the emission code
    check(current_code != "zzz", "Cannot increment emission code beyond 'zzz'");

    string next_code = increment_code(current_code);
    string next_symbol_str = org_code + next_code;

    symbol next_symbol(symbol_code(next_symbol_str), 0);

    _emissioncodes.modify(it, get_self(), [&](auto& row) {
        row.last_emission_symbol = next_symbol;
    });
}

    // Action to compute and update the next bounty code
ACTION nextbounty(name org) {
    string action_name = "nextbounty";
    string failure_identifier = "CONTRACT: org, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    bountycode_table _bountycodes(get_self(), get_self().value);
    orgcode_table _orgcodes(get_self(), get_self().value);

    auto org_it = _orgcodes.find(org.value);
    check(org_it != _orgcodes.end(), "Organization code not found for this organization");

    auto it = _bountycodes.find(org.value);
    check(it != _bountycodes.end(), "Bounty code not initialized for this organization");

    string org_code = org_it->org_code.to_string();
    string current_code = it->last_bounty_symbol.code().to_string();

    // Ensure we can increment the bounty code
    check(current_code != "zzz", "Cannot increment bounty code beyond 'zzz'");

    string next_code = increment_code(current_code);
    string next_symbol_str = org_code + next_code;

    symbol next_symbol(symbol_code(next_symbol_str), 0);

    _bountycodes.modify(it, get_self(), [&](auto& row) {
        row.last_bounty_symbol = next_symbol;
    });
}
