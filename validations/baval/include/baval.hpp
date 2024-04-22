#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define BOUNDED_AGG_MANAGER_CONTRACT "bamanagerzzz"

#define BOUNDED_AGG_MANAGER_INIT_AGG_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::initagg"
#define BOUNDED_AGG_MANAGER_INIT_SEQ_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::initseq"
#define BOUNDED_AGG_MANAGER_ACTIVATE_SEQUENCE_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::actseq"
#define BOUNDED_AGG_MANAGER_ACTIVATE_ALL_INIT_SEQUENCE_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::actseqai"
#define BOUNDED_AGG_MANAGER_ACTIVATE_FIRST_INIT_SEQUENCE_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::actseqfi"
#define BOUNDED_AGG_MANAGER_END_SEQUENCE_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::endseq"
#define BOUNDED_AGG_MANAGER_END_ALL_ACTIVE_SEQUENCE_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::endseqaa"
#define BOUNDED_AGG_MANAGER_END_FIRST_ACTIVE_SEQUENCE_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::endseqfa"
#define BOUNDED_AGG_MANAGER_ADD_BADGES_NOTIFICATION BOUNDED_AGG_MANAGER_CONTRACT"::addbadge"

CONTRACT baval : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(BOUNDED_AGG_MANAGER_INIT_AGG_NOTIFICATION)]]
    void initagg(name authorized, name org, name agg, string agg_description);

    [[eosio::on_notify(BOUNDED_AGG_MANAGER_INIT_SEQ_NOTIFICATION)]]
    void initseq(name authorized, name org, name agg, string sequence_description);

    [[eosio::on_notify(BOUNDED_AGG_MANAGER_ACTIVATE_SEQUENCE_NOTIFICATION)]]
    void actseq(name authorized, name org, name agg, vector<uint64_t> seq_ids, vector<name> badges);
    
    [[eosio::on_notify(BOUNDED_AGG_MANAGER_ACTIVATE_ALL_INIT_SEQUENCE_NOTIFICATION)]]
    void actseqai(name authorized, name org, name agg, vector<name> badges);
    
    [[eosio::on_notify(BOUNDED_AGG_MANAGER_ACTIVATE_FIRST_INIT_SEQUENCE_NOTIFICATION)]]
    void actseqfi(name authorized, name org, name agg, vector<name> badges);
    
    [[eosio::on_notify(BOUNDED_AGG_MANAGER_END_SEQUENCE_NOTIFICATION)]]
    void endseq(name authorized, name org, name agg, vector<uint64_t> seq_ids);
    
    [[eosio::on_notify(BOUNDED_AGG_MANAGER_END_ALL_ACTIVE_SEQUENCE_NOTIFICATION)]]
    void endseqaa(name authorized, name org, name agg);
    
    [[eosio::on_notify(BOUNDED_AGG_MANAGER_END_FIRST_ACTIVE_SEQUENCE_NOTIFICATION)]]
    void endseqfa(name authorized, name org, name agg);
    
    [[eosio::on_notify(BOUNDED_AGG_MANAGER_ADD_BADGES_NOTIFICATION)]]
    void addbadge(name authorized, name org, name agg, vector<uint64_t> seq_ids, vector<name> badges);


    ACTION addaggauth(name org, name action, name agg, name authorized_account);
    ACTION delaggauth(name org, name action, name agg, name authorized_account);
    ACTION addactionauth (name org, name action, name authorized_account);
    ACTION delactionauth (name org, name action, name authorized_account);
  private:

    TABLE actionauths {
      name  action;
      vector<name>  authorized_accounts;
      auto primary_key() const { return action.value; }
    };
    typedef multi_index<name("actionauths"), actionauths> actionauths_table;

    TABLE aggauths {
      uint64_t id;
      name action;
      name agg;
      vector<name> authorized_accounts;

      uint64_t primary_key() const { return id; }
      uint128_t secondary_key() const { return combine_names(action, agg); }

      static uint128_t combine_names(const name& a, const name& b) {
          return (uint128_t{a.value} << 64) | b.value;
      }
    };

    typedef eosio::multi_index<"aggauths"_n, aggauths,
        indexed_by<"byactionagg"_n, const_mem_fun<aggauths, uint128_t, &aggauths::secondary_key>>
    > aggauths_table;

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

    bool has_agg_authority (name org, name action, name agg, name account) {
      aggauths_table aggauths(get_self(), org.value);
      auto secondary_index = aggauths.get_index<"byactionagg"_n>();
      uint128_t secondary_key = aggauths::combine_names(action, agg);
      auto itr = secondary_index.find(secondary_key);
      if(itr == secondary_index.end() || itr->action != action || itr->agg != agg) {
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
