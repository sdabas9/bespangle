#include <seriesbadge.hpp>
#include <json.hpp>

using json = nlohmann::json;

    // todo 
    // 1) check for cycles to throw better error message, c.
    // 2) replace values in error messages.
    // 3) put profiles contract name in a global constant
    // 4) add action to update image json.
    // 5) add action to update details json.


  ACTION seriesbadge::define(name org, name family) {
    require_auth(org);
    metadata_table _metadata (_self, org.value);
    auto metadata_itr = _metadata.find(family.value);

    check(metadata_itr == _metadata.end(), "<contractname><actionname> : <family> already exists");
    
    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT_NAME),
      name("isrecognized"),
      isrecognized_args {
        .org = org,
        .contract = get_self()}
    }.send();
    
    _metadata.emplace(org, [&](auto& row) {
      row.family = family;
      row.seq_id = 0;
    });
  }

  ACTION seriesbadge::createnext(name org, name family, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
    require_auth(org);
    metadata_table _metadata (_self, org.value);
    auto metadata_itr = _metadata.find(family.value);
    check(metadata_itr != _metadata.end(), "<contractname><actionname> : <family> does not exist");
    
 
    // insert into badge table
    uint64_t new_seq_id = metadata_itr->seq_id + 1;
    _metadata.modify(metadata_itr, org, [&](auto& row) {
      row.seq_id = new_seq_id;
    });

    badge_table _badge (_self, org.value);
    auto family_badge_index = _badge.get_index<name("familybadge")>();
    uint128_t family_badge_key = ((uint128_t) family.value) << 64 | badge.value;
    auto family_badge_iterator = family_badge_index.find (family_badge_key);
    check(family_badge_iterator == family_badge_index.end(), "<family> <badge> exists");

    _badge.emplace(org, [&](auto& row){
      row.id = _badge.available_primary_key();
      row.family = family;
      row.seq_id = new_seq_id;
      row.badge = badge;
    }); 
    
    auto _onchain_lookup_data = json::parse(onchain_lookup_data);
    _onchain_lookup_data["_family"] = (metadata_itr->family).to_string();
    _onchain_lookup_data["_seq_id"] = new_seq_id;
    
    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT_NAME),
      name("initbadge"),
      initbadge_args {
        .org = org,
        .badge_contract = get_self(),
        .badge_name = badge,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = _onchain_lookup_data.dump(),
        .memo = memo }
    }.send();
  }

  ACTION seriesbadge::issuelatest(name org, name family, name to, string memo) {
    metadata_table _metadata (_self, org.value);
    auto metadata_itr = _metadata.find(family.value);
    check(metadata_itr != _metadata.end(), "<family> not defined. Call define action");
    
    uint64_t seq_id = metadata_itr->seq_id;
    check(seq_id != 0, "Create badge for the family.");
    
    badge_table _badge (_self, org.value);
    auto family_seq_id_index = _badge.get_index<name("familyseqid")>();
    uint128_t family_seq_id_key = ((uint128_t) seq_id) << 64 | family.value;
    auto family_seq_id_iterator = family_seq_id_index.find (family_seq_id_key);

    name badge_name = family_seq_id_iterator->badge;

    // get badge name 
    // issue id
    require_auth(org); 
      action {
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT_NAME),
        name("achievement"),
        achievement_args {
          .org = org,
          .badge_contract = get_self(),
          .badge_name = badge_name,
          .account = to,
          .from = org,
          .count = 1,
          .memo = memo }
      }.send();
  }


    

    
