#include <deferred.hpp>

ACTION deferred::addrecord(name org, name contract, name from, name to, uint64_t amount, std::string memo, name badge, time_point_sec execution_time) {
  require_auth(get_self());

  eosio::check(execution_time >= current_time_point(), "Execution time cannot be in the past");

  records_table records(get_self(), org.value);
  auto iterator = records.available_primary_key();

  uint64_t hour = get_hour_since_epoch(execution_time);

  records.emplace(get_self(), [&](auto& row) {
    row.id = iterator;
    row.hour = hour;
    row.contract = contract;
    row.from = from;
    row.to = to;
    row.amount = amount;
    row.memo = memo;
    row.badge = badge;
    row.created_at = current_time_point();
  });
}

ACTION deferred::execute(name org, uint64_t record_id) {
  require_auth(get_self());

  records_table records(get_self(), org.value);
  auto iterator = records.find(record_id);
  eosio::check(iterator != records.end(), "Record with the given ID does not exist");

  uint64_t current_hour = get_current_hour();
  eosio::check(current_hour >= iterator->hour, "Cannot execute before the specified hour");

  // Execute the inline action
  action(
    permission_level{get_self(), "active"_n},
    iterator->contract, "issue"_n,
    issue_args{org, iterator->badge, iterator->from, iterator->to, iterator->amount, iterator->memo}
  ).send();

  // Erase the record from the table
  records.erase(iterator);
}

ACTION deferred::execbyhour(name org, uint64_t hour, uint64_t step) {
  require_auth(get_self());

  records_table records(get_self(), org.value);

  auto houridx = records.get_index<name("hourindex")>();
  auto iterator = houridx.find(hour);

  uint64_t count = 0;
  while (iterator != houridx.end() && iterator->hour == hour && count < step) {
    auto next = iterator;
    ++next;
    execute(org, iterator->id);
    iterator = next;
    ++count;
  }
}

EOSIO_DISPATCH(deferred, (addrecord)(execute)(execbyhour))
