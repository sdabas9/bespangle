#include <rounds.hpp>

void rounds::notifyachiev (
    name org, 
    name badge_contract, 
    name badge_name,
    name account,
    name from, 
    uint8_t count,
    string memo,
    uint64_t badge_id,  
    vector<name> notify_accounts) {

    scoremeta_table _scoremeta (get_self(), org.value);
    
    auto badge_round_status_index = _scoremeta.get_index<name("badgestatus")>();
    uint128_t badge_round_status_key = ((uint128_t) badge_id) << 64 | name("notended").value;
    auto badge_round_status_iterator = badge_round_status_index.find (badge_round_status_key);

    while (
      (badge_round_status_iterator != badge_round_status_index.end()) &&
      (badge_round_status_iterator->badge_id == badge_id) &&
      (badge_round_status_iterator->round_status_tentative == name("notended"))) {

      rounds_table _rounds (get_self(), org.value);
      auto rounds_itr = _rounds.find(badge_round_status_iterator->round.value);

      if (rounds_itr != _rounds.end()) {
        if (rounds_itr->round_status == name("ended")) {
          badge_round_status_index.modify(badge_round_status_iterator, get_self(), [&](auto& row) {
            row.round_status_tentative = name("ended");
          });
        } 
        if (rounds_itr->round_status == name("active")) {
            switch(badge_round_status_iterator->balance_based_scoring_type) {
            case name("linear"): 
              balance_based_linear_score (
                org,
                badge_round_status_iterator->scoremeta_id,
                account,
                count,
                badge_round_status_iterator->balance_based_scoring_weight);
              break;
            case name("fibonacci"):
              balance_based_fibo_score (
                org,
                badge_round_status_iterator->scoremeta_id,
                account,
                count,
                badge_round_status_iterator->balance_based_scoring_weight);
              break;
            default: ;
          }
          switch(badge_round_status_iterator->source_based_scoring_type) {
            case name("linear"):
              source_based_linear_score (
                org,
                badge_round_status_iterator->scoremeta_id,
                account,
                from,
                badge_round_status_iterator->source_based_scoring_weight);
              break;
            case name("fibonacci"):
              source_based_fibo_score (
                org,
                badge_round_status_iterator->scoremeta_id,
                account,
                from,
                badge_round_status_iterator->source_based_scoring_weight);
              break;
            default: ;
          }
          // insert total scores, badge scores
        }
      }
      ++badge_round_status_iterator;
    } 


  }

ACTION rounds::createround (name org,
  name round,
  string description,
  bool account_constrained,
  vector<name> participating_accounts) {

  require_auth(org);
  rounds_table _rounds( _self, org.value );
  auto rounds_itr = _rounds.find(round.value);
  check(rounds_itr == _rounds.end(), "<round> already exists");

  _rounds.emplace(get_self(), [&](auto& row){
    row.round = round;
    row.description = description;
    row.account_constrained = account_constrained;
    row.participating_accounts = participating_accounts;
    row.round_status = name("init");
  });

}

ACTION rounds::startround (name org, name round) {
  require_auth(org);
  rounds_table _rounds( _self, org.value );
  auto rounds_itr = _rounds.find(round.value);
  check(rounds_itr != _rounds.end(), "<round> does not exist, create round using <createround> action");
  check(rounds_itr->round_status != name("active"), "<round> already in active status");
  check(rounds_itr->round_status != name("ended"), "<round> status cannot be changed to active from ended");
  _rounds.modify(rounds_itr, get_self(), [&](auto& row) {
    row.round_status = name("active");
  });
}

ACTION rounds::endround (name org, name round) {
  require_auth(org);
  rounds_table _rounds( _self, org.value );
  auto rounds_itr = _rounds.find(round.value);
  check(rounds_itr != _rounds.end(), "<round> does not exist, create round using <createround> action");
  _rounds.modify(rounds_itr, get_self(), [&](auto& row) {
    row.round_status = name("ended");
  });
}

ACTION rounds::pauseround (name org, name round) {
  require_auth(org);
  rounds_table _rounds( _self, org.value );
  auto rounds_itr = _rounds.find(round.value);
  check(rounds_itr != _rounds.end(), "<round> does not exist, create round using <createround> action");
  check(rounds_itr->round_status == name("active"), "can only pause from active status");
  
  _rounds.modify(rounds_itr, get_self(), [&](auto& row) {
    row.round_status = name("paused");
  });
}

ACTION rounds::resumeround (name org, name round) {
  require_auth(org);
  rounds_table _rounds( _self, org.value );
  auto rounds_itr = _rounds.find(round.value);
  check(rounds_itr != _rounds.end(), "<round> does not exist, create round using <createround> action");
  check(rounds_itr->round_status == name("pause"), "can only resume from pause status");
  
  _rounds.modify(rounds_itr, get_self(), [&](auto& row) {
    row.round_status = name("resume");
  });

}

ACTION rounds::addscoremeta (name org,
  name round, 
  uint64_t badge_id, 
  name balance_based_scoring_type, 
  uint16_t balance_based_scoring_weight,
  name source_based_scoring_type,
  uint16_t source_based_scoring_weight) {

  require_auth(org);
  rounds_table _rounds ( _self, org.value ); 
  auto rounds_itr = _rounds.find(round.value);
  check(rounds_itr != _rounds.end(), "<round> does not exist, create round using <createround> action");
  check(rounds_itr->round_status != name("ended"), "<round> already ended");
  
  scoremeta_table _scoremeta (_self, org.value);

  _scoremeta.emplace(get_self(), [&](auto& row){
    row.scoremeta_id = _scoremeta.available_primary_key();
    row.round = round;
    row.badge_id = badge_id;
    row.round_status_tentative = name("notended");
    row.balance_based_scoring_type = balance_based_scoring_type;
    row.balance_based_scoring_weight = balance_based_scoring_weight;
    row.source_based_scoring_type = source_based_scoring_type;
    row.source_based_scoring_weight = source_based_scoring_weight;
  });  
}


