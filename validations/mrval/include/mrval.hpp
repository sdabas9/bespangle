#include <eosio/eosio.hpp>
#include <vector>

using namespace std;
using namespace eosio;
using std::vector;

#define MUTUAL_RECOGNITION_INTERFACE_CONTRACT "mrmanagerxxx"

#define MUTUAL_RECOGNITION_INTERFACE_CREATE_NOTIFICATION MUTUAL_RECOGNITION_INTERFACE_CONTRACT"::initsimple" 


CONTRACT mrval : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(MUTUAL_RECOGNITION_INTERFACE_CREATE_NOTIFICATION)]] void initgotcha (name org,
      name creator, 
      name badge, 
      time_point_sec starttime, 
      uint64_t cycle_length, 
      uint8_t supply_per_cycle, 
      string offchain_lookup_data, 
      string onchain_lookup_data,
      vector<name> consumers,
      string memo);
      
    ACTION addactionauth (name org, name action, name authorized_account);

    ACTION delactionauth (name org, name action, name authorized_account);


  private:
    TABLE actionauths {
      name  action;
      vector<name>  authorized_accounts;
      auto primary_key() const { return action.value; }
    };
    typedef multi_index<name("actionauths"), actionauths> actionauths_table;



    bool has_action_authority (name org, name action, name account) {
      actionauths_table _actionauths_table (get_self(), org.value);
      auto itr = _actionauths_table.find(action.value);
      if(itr == _actionauths_table.end()) {
        return false;
      }
      auto authorized_accounts = itr->authorized_accounts;
      for(auto i = 0; i < authorized_accounts.size(); i++) {
        if(authorized_accounts[i] == account) {
          return true;
        }
      }
      return false;
    }

};
