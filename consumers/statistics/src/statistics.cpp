#include <statistics.hpp>


void statistics::notifyachiev(
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts) {
    string action_name = "notifyachiev";
    string failure_identifier = "CONTRACT: statistics, ACTION: " + action_name + ", MESSAGE: ";
    keystats_table _keystats( _self, _self.value );
    auto sym = badge_asset.symbol;  
    auto keystats_itr = _keystats.find(sym.code().raw());
    

    if(keystats_itr == _keystats.end()) {
      _keystats.emplace(get_self(), [&](auto& row){
        row.badge_symbol = badge_asset.symbol;
        row.max_issued = badge_asset.amount;
        row.account_count = 1;
        row.total_issued = badge_asset.amount;
      });
    } else {

      accounts _accounts( name(CUMULATIVE_CONTRACT), to.value );
      auto accounts_itr = _accounts.find(sym.code().raw());
      check(accounts_itr != _accounts.end(), failure_identifier + "possibly cumulative consumer is not followed by stats consumer");
      uint64_t balance = accounts_itr->balance.amount;
      uint64_t max;
      uint64_t account_count;
      uint64_t total; 
      if(balance > keystats_itr->max_issued) {
        max = balance;
      } else {
        max = keystats_itr->max_issued;
      }
      if(balance == badge_asset.amount) {
        account_count = keystats_itr->account_count + 1;
      } else {
        account_count = keystats_itr->account_count;
      }
      total = keystats_itr->total_issued + badge_asset.amount;

      _keystats.modify(keystats_itr, get_self(), [&](auto& row) {
        row.max_issued = max;
        row.account_count = account_count;
        row.total_issued = total;
      });


    }
}

ACTION statistics::bootstrap(symbol badge_symbol, 
      uint64_t max_issued,
      uint64_t account_count, 
      uint64_t total_issued) {

    string action_name = "bootstrap";
    string failure_identifier = "CONTRACT: statistics, ACTION: " + action_name + ", MESSAGE: ";
 
    check_internal_auth(name("bootstrap"), failure_identifier);
    keystats_table _keystats( _self, _self.value );  
    auto keystats_itr = _keystats.find(badge_symbol.code().raw());

    if(keystats_itr == _keystats.end()) {
      _keystats.emplace(get_self(), [&](auto& row){
        row.badge_symbol = badge_symbol;
        row.max_issued = max_issued;
        row.account_count = account_count;
        row.total_issued = total_issued;
      });
    } else {
      _keystats.modify(keystats_itr, get_self(), [&](auto& row) {
        row.max_issued = max_issued;
        row.account_count = account_count;
        row.total_issued = total_issued;
      });
    }

}

