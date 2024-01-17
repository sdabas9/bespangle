#include <orgbill.hpp>

// todo 
// 1) check for system currency in buycredits notified action.
// 2) buyram action from system currency.
// 3) put profiles contract in a variable.
// 4) error messages replace with value.
// 5) org's account to notify when credits run out.
// 6) separate out contract authorization and multisig authorization.
// recognize and addsettings are multisig authorizations.
// notify is contract authorization.


ACTION orgbill::addsettings (name key, uint32_t value) {
  check_internal_auth(name("addsettings"));
  settings_table _settings(get_self(), get_self().value);
  auto itr = _settings.find(key.value);
  if(itr == _settings.end()) {
    _settings.emplace(get_self(), [&](auto& row) {
      row.key = key;
      row.value = value;
    });
  } else {
    _settings.modify(itr, get_self(), [&](auto& row) {
      row.value = value;
    });
  }
}

void orgbill::buycredits(name from, name to, asset quantity, string memo) {

  if (to != get_self() || from == get_self()){
    eosio::print("Ignoring token transfer of contract to itself and from itself.");
    return;
  }
  credits_table _credits(get_self(), get_self().value);
  auto itr = _credits.find(name(memo).value);
  uint32_t credits_bought = token_amount_to_credits (quantity.amount);
  uint32_t max_credit_balance = getvalue(name(MAX_CREDIT_BALANCE)); 
  if(itr == _credits.end()) {
    
    check(credits_bought <= max_credit_balance, "billing contract :can not buy more than 10000 credits");
    _credits.emplace(get_self(), [&](auto& row) {
      row.org = name(memo);
      row.total_credits = credits_bought;
      row.used_credits = 1;
    });
  } else {
    _credits.modify(itr, get_self(), [&](auto& row) {
      check(row.total_credits + credits_bought <= max_credit_balance, "billing contract : can not hold more than 10000 credits");
      row.total_credits = row.total_credits + credits_bought;
    });
  }
}

ACTION orgbill::protocolfees(name org, name feature) {
  check_internal_auth(name("protocolfees"));
  uint32_t credits = getvalue(feature);
  deduct_credit (org, credits);
}

ACTION orgbill::ramcredits(name org, name contract, uint64_t bytes, string memo) {
  check_internal_auth(name("ramcredits"));
  uint32_t credits = bytes_to_credits(bytes);
  deduct_credit (org, credits);
}

ACTION orgbill::notify (name org, uint32_t total_credits, uint32_t used_credits) {
  require_auth(get_self());
  require_recipient(org);
}

