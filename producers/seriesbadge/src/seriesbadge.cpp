#include <seriesbadge.hpp>
#include <json.hpp>

using json = nlohmann::json;

void seriesbadge::extcseries(name org, name series) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("createseries"),
    createseries_args {
      .org = org,
      .series = series }
  }.send();  
}

void seriesbadge::extcnext(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("createnext"),
    createnext_args {
      .org = org,
      .series = series,
      .badge = badge,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo }
  }.send();  
}

void seriesbadge::extilatest(name org, name series, name to, uint64_t count, string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("issuelatest"),
    issuelatest_args {
      .org = org,
      .series = series,
      .to = to,
      .count = count,
      .memo = memo }
  }.send();    
}

void seriesbadge::extiany(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("issueany"),
    issueany_args {
      .org = org,
      .series = series,
      .seq_id = seq_id,
      .to = to,
      .count = count,
      .memo = memo }
  }.send();      
}

ACTION seriesbadge::createseries(name org, name series) {

  require_auth(get_self());
  metadata_table _metadata (_self, org.value);
  auto metadata_itr = _metadata.find(series.value);

  check(metadata_itr == _metadata.end(), "<contractname><actionname> : <series> already exists");
      
  _metadata.emplace(get_self(), [&](auto& row) {
    row.series = series;
    row.seq_id = 0;
  });
}

ACTION seriesbadge::createnext(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
  require_auth(get_self());
  metadata_table _metadata (_self, org.value);
  auto metadata_itr = _metadata.find(series.value);
  check(metadata_itr != _metadata.end(), "<contractname><actionname> : <series> does not exist");
  

  // insert into badge table
  uint64_t new_seq_id = metadata_itr->seq_id + 1;
  _metadata.modify(metadata_itr, get_self(), [&](auto& row) {
    row.seq_id = new_seq_id;
  });

  badge_table _badge (_self, org.value);
  auto series_badge_index = _badge.get_index<name("seriesbadge")>();
  uint128_t series_badge_key = ((uint128_t) series.value) << 64 | badge.value;
  auto series_badge_iterator = series_badge_index.find (series_badge_key);
  check(series_badge_iterator == series_badge_index.end(), "<series> <badge> exists");

  _badge.emplace(get_self(), [&](auto& row){
    row.id = _badge.available_primary_key();
    row.series = series;
    row.seq_id = new_seq_id;
    row.badge = badge;
  }); 
  
  auto _onchain_lookup_data = json::parse(onchain_lookup_data);
  _onchain_lookup_data["_series"] = (metadata_itr->series).to_string();
  _onchain_lookup_data["_seq_id"] = new_seq_id;
  
  action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT_NAME),
    name("initbadge"),
    initbadge_args {
      .org = org,
      .badge_name = badge,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = _onchain_lookup_data.dump(),
      .memo = memo }
  }.send();
}

ACTION seriesbadge::issuelatest(name org, name series, name to, uint64_t count, string memo) {
  require_auth(get_self()); 
  metadata_table _metadata (_self, org.value);
  auto metadata_itr = _metadata.find(series.value);
  check(metadata_itr != _metadata.end(), "<series> not defined. Call define action");
  
  uint64_t seq_id = metadata_itr->seq_id;
  check(seq_id != 0, "Create badge for the series.");
  
  badge_table _badge (_self, org.value);
  auto series_seq_id_index = _badge.get_index<name("seriesseqid")>();
  uint128_t series_seq_id_key = ((uint128_t) seq_id) << 64 | series.value;
  auto series_seq_id_iterator = series_seq_id_index.find (series_seq_id_key);

  name badge_name = series_seq_id_iterator->badge;

  // get badge name 
  // issue id

  action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT_NAME),
    name("achievement"),
    achievement_args {
      .org = org,
      .badge_name = badge_name,
      .account = to,
      .from = org,
      .count = count,
      .memo = memo }
  }.send();
}

ACTION seriesbadge::issueany(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo) {
  require_auth(get_self()); 
  metadata_table _metadata (_self, org.value);
  auto metadata_itr = _metadata.find(series.value);
  check(metadata_itr != _metadata.end(), "<series> not defined. Call define action");

  badge_table _badge (_self, org.value);
  auto series_seq_id_index = _badge.get_index<name("seriesseqid")>();
  uint128_t series_seq_id_key = ((uint128_t) seq_id) << 64 | series.value;
  auto series_seq_id_iterator = series_seq_id_index.find (series_seq_id_key);

  check(series_seq_id_iterator!=series_seq_id_index.end() && 
    series_seq_id_iterator->series==series &&
    series_seq_id_iterator->seq_id==seq_id, "<series><seq_id> combination invalid ");

  name badge_name = series_seq_id_iterator->badge;

  action {
    permission_level{get_self(), name("active")},
    name(ORCHESTRATOR_CONTRACT_NAME),
    name("achievement"),
    achievement_args {
      .org = org,
      .badge_name = badge_name,
      .account = to,
      .from = org,
      .count = count,
      .memo = memo }
  }.send();
}


    

    
