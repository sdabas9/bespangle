#include <elections.hpp>

ACTION elections::vote(name voter) {
  require_auth(voter);

  events_table _events (_self, _self.value);
  auto events_itr = _events.find(name("vote").value);

  check(events_itr != _events.end(), "init vote event with name vote");
  name badge = events_itr->badge;

  string memo = "";
  action {
      permission_level{get_self(), name("active")},
      name(EDEN_ORG_INTERFACE_CONTRACT),
      name("givesimple"),
      givesimple_args {
        .badge = badge,
        .authorizer = get_self(),
        .to = voter,
        .memo = memo }
  }.send();

}

ACTION elections::initevent(name event, name badge) {
  require_auth(get_self());
  
  events_table _events (_self, _self.value);
  auto events_itr = _events.find(event.value);

  check(events_itr == _events.end(), "event already exists");

  _events.emplace(get_self(), [&](auto& row) {
    row.event = event;
    row.badge = badge;
  });
}
