#include <seriesnotify.hpp>

void seriesnotify::acreateserie(name org, name series) {
  action {
    permission_level{get_self(), name("active")},
    name(get_self()),
    name("createseries"),
    createseries_args {
      .org = org,
      .series = series }
  }.send();  
}

ACTION seriesnotify::createseries(name org, name series) {
  require_auth(get_self());
  require_recipient(name(SERIESBADGE_CONTRACT_NAME));
}

void seriesnotify::acreatenex(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
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

ACTION seriesnotify::createnext(name org, name series, name badge, string offchain_lookup_data, string onchain_lookup_data, string memo) {
  require_auth(get_self());
  require_recipient(name(SERIESBADGE_CONTRACT_NAME));    
}


void seriesnotify::aissuelates(name org, name series, name to, uint64_t count, string memo) {
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

ACTION seriesnotify::issuelatest(name org, name series, name to, uint64_t count, string memo) {
  require_auth(get_self());
  require_recipient(name(SERIESBADGE_CONTRACT_NAME));    
}

void seriesnotify::aissuean(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo) {
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

ACTION seriesnotify::issueany(name org, name series, uint64_t seq_id, name to, uint64_t count, string memo) {
  require_auth(get_self());
  require_recipient(name(SERIESBADGE_CONTRACT_NAME));      
}
