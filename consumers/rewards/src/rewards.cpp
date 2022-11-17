#include <rewards.hpp>

ACTION rewards::test(name fn, name sn) {
  require_auth(from);

}

ACTION rewards::clear() {
  require_auth(get_self());

  messages_table _messages(get_self(), get_self().value);

  // Delete all records in _messages table
  auto msg_itr = _messages.begin();
  while (msg_itr != _messages.end()) {
    msg_itr = _messages.erase(msg_itr);
  }
}

EOSIO_DISPATCH(rewards, (hi)(clear))
