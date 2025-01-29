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

// Action: Update organization image
ACTION org::updateimage(name org, string org_ipfs_image) {
    require_auth(org);

    // Access orgcode table to check if the organization exists
    orgcode_index orgcodes(get_self(), get_self().value);
    auto orgcode_itr = orgcodes.find(org.value);
    check(orgcode_itr != orgcodes.end(), "Organization not found in orgcode table");

    // Access orginfo table to fetch or insert the organization
    orginfo_table orginfos(get_self(), get_self().value);
    auto orginfo_itr = orginfos.find(org.value);

    if (orginfo_itr == orginfos.end()) {
        // Insert a new record if it doesn't exist
        orginfos.emplace(get_self(), [&](auto& row) {
            row.org = org;
            row.org_ipfs_image = org_ipfs_image;
        });
    } else {
        // Update the IPFS image if the record exists
        orginfos.modify(orginfo_itr, get_self(), [&](auto& row) {
            row.org_ipfs_image = org_ipfs_image;
        });
    }
}

