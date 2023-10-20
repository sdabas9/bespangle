#include <deferred.hpp>



void deferred::rxsetstat(name org, name round, name status, time_point execution_time) {
  action(
    permission_level{get_self(), "active"_n},
    get_self(), "rqsetstat"_n,
    rqsetstat_args{.org = org, .round = round, .status = status, .execution_time = execution_time}
  ).send();
}


void deferred::rxaddbgrnd(name org, name round, name badge, time_point execution_time) {
  action(
    permission_level{get_self(), "active"_n},
    get_self(), "rqaddbgrnd"_n,
    rqaddbgrnd_args{.org = org, .round = round, .badge = badge, .execution_time = execution_time}
  ).send();
}


void deferred::rxrembgrnd(name org, name round, name badge, time_point execution_time) {
  action(
    permission_level{get_self(), "active"_n},
    get_self(), "rqrembgrnd"_n,
    rqrembgrnd_args{.org = org, .round = round, .badge = badge, .execution_time = execution_time}
  ).send();
} 


ACTION deferred::rqsetstat(name org, name round, name status, time_point execution_time) {
  require_auth(get_self());

  eosio::time_point tp = eosio::current_time_point();
  eosio::microseconds curr_micros = tp.time_since_epoch();
  uint64_t count_curr_micros = curr_micros.count();
  uint64_t count_exec_micros = execution_time.elapsed.count();
  eosio::check(count_exec_micros >= count_curr_micros(), "Execution time cannot be in the past");

  rounds_table _rounds(get_self(), org.value);

  _rounds.emplace(get_self(), [&](auto& row) {
    row.id = _rounds.available_primary_key();
    row.execution_time_in_micro_sec_since_epoch = count_exec_micros;
    row.action = name("setstat");
    row.param1 = round;
    row.param2 = status;
  });

}

ACTION deferred::rqaddbgrnd(name org, name round, name badge, time_point execution_time) {
  require_auth(get_self());

  eosio::time_point tp = eosio::current_time_point();
  eosio::microseconds curr_micros = tp.time_since_epoch();
  uint64_t count_curr_micros = curr_micros.count();
  uint64_t count_exec_micros = execution_time.elapsed.count();
  eosio::check(count_exec_micros >= count_curr_micros(), "Execution time cannot be in the past");

  rounds_table _rounds(get_self(), org.value);

  _rounds.emplace(get_self(), [&](auto& row) {
    row.id = _rounds.available_primary_key();
    row.execution_time_in_micro_sec_since_epoch = count_exec_micros;
    row.action = name("addbgrnd");
    row.param1 = round;
    row.param2 = badge;
  });

}

ACTION deferred::rqrembgrnd(name org, name round, name status, time_point execution_time) {
  require_auth(get_self());

  eosio::time_point tp = eosio::current_time_point();
  eosio::microseconds curr_micros = tp.time_since_epoch();
  uint64_t count_curr_micros = curr_micros.count();
  uint64_t count_exec_micros = execution_time.elapsed.count();
  eosio::check(count_exec_micros >= count_curr_micros(), "Execution time cannot be in the past");

  rounds_table _rounds(get_self(), org.value);

  _rounds.emplace(get_self(), [&](auto& row) {
    row.id = _rounds.available_primary_key();
    row.execution_time_in_micro_sec_since_epoch = count_exec_micros;
    row.action = name("rembgrnd");
    row.param1 = round;
    row.param2 = badge;
  });

}

ACTION deferred::rounddeque(name org, uint64_t step, name executing_account) {
  require_auth(get_self());
  eosio::time_point tp = eosio::current_time_point();
  eosio::microseconds micros = tp.time_since_epoch();
  uint64_t count_micros = micros.count();
  
  rounds_table _rounds(get_self(), org.value);
  auto micro_index = _rounds.get_index<name("microindex")>();
  auto lower = micro_index.lower_bound(1);
  auto upper = micro_index.upper_bound(count_micros);

  uint16_t processed = 0;
  for(auto itr = lower; itr != upper; ++itr) {

    if(itr->action == name("setstat")) {

      action(
        permission_level{get_self(), "active"_n},
        get_self(), "resetstat"_n,
        setstat_args{.org = org, .round = itr->param1, .status = itr->param2}
      ).send();

    } else if (itr->action == name("addbgrnd")) {

      action(
        permission_level{get_self(), "active"_n},
        get_self(), "readdbgrnd"_n,
        addbgrnd_args{.org = org, .round = itr->param1, .badge = itr->param2}
      ).send();
    
    } else if (itr->action == name("rembgrnd")) {

      action(
        permission_level{get_self(), "active"_n},
        get_self(), "rerembgrnd"_n,
        rembgrnd_args{.org = org, .round = itr->param1, .badge = itr->param2}
      ).send();
    
    }

    processed++;
    micro_index.erase(itr);

    if(processed == step) {
      return;
    }
  }
  check(processed == 0, "Nothing to process");
}

ACTION deferred::resetstat(name org, name round, name status) {
  require_recipient(notification_contract);
}
ACTION deferred::readdbgrnd(name org, name round, name badge) {
  require_recipient(notification_contract);
}
ACTION deferred::rerembgrnd(name org, name round, name badge) {
  require_recipient(notification_contract);
}

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
