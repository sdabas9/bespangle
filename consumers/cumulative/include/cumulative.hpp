#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define BILLING_CONTRACT "billingxxxxx"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define SUBSCRIPTION_CONTRACT "subsyyyyyyyy"


#define NEW_BADGE_ISSUANCE_NOTIFICATION ORCHESTRATOR_CONTRACT"::notifyachiev"


CONTRACT cumulative : public contract {
public:
  using contract::contract;
  
  [[eosio::on_notify(NEW_BADGE_ISSUANCE_NOTIFICATION)]] void notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts);

  ACTION dummy();

private:
  struct ramcredits_arg {
    name org;
    name contract;
    uint64_t bytes;
    string memo;
  };

  TABLE account {
    asset    balance;
    uint64_t primary_key() const { return balance.symbol.code().raw(); }
  };
  typedef eosio::multi_index<"accounts"_n, account> accounts;

  void deduct_credit(name org, uint32_t bytes, string memo) {
    action{
      permission_level{get_self(), "active"_n},
      name(BILLING_CONTRACT),
      "ramcredits"_n,
      ramcredits_arg{
        .org = org,
        .contract = get_self(),
        .bytes = bytes,
        .memo = memo
      }
    }.send();
  }

  struct billing_args {
    name org;
    uint8_t actions_used;
  };

};

