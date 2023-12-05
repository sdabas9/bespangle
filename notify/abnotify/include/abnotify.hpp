#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ANTIBADGE_INTERFACE_CONTRACT_NAME "abmanagerxxx"
#define ANTIBADGE_CONTRACT_NAME "antibadgexxx"

#define ANTIBADGE_INTERFACE_CREATE_NOTIFICATION ANTIBADGE_INTERFACE_CONTRACT_NAME"::create"
#define ANTIBADGE_INTERFACE_CREATE_INV_NOTIFICATION ANTIBADGE_INTERFACE_CONTRACT_NAME"::createinv"
#define ANTIBADGE_INTERFACE_ISSUE_NOTIFICATION ANTIBADGE_INTERFACE_CONTRACT_NAME"::issue"

CONTRACT abnotify : public contract {
  public:
    using contract::contract;

    [[eosio::on_notify(ANTIBADGE_INTERFACE_CREATE_NOTIFICATION)]]
    void acreate(name org,
                   name antibadge,
                   name badge,
                   name type,
                   string offchain_lookup_data,
                   string onchain_lookup_data,
                   string memo);

    [[eosio::on_notify(ANTIBADGE_INTERFACE_CREATE_INV_NOTIFICATION)]]
    void acinv(name org,
                 name antibadge,
                 name badge,
                 string offchain_lookup_data,
                 string onchain_lookup_data,
                 string memo);

    [[eosio::on_notify(ANTIBADGE_INTERFACE_ISSUE_NOTIFICATION)]]
    void aissue(name org,
                  name to,
                  name antibadge,
                  uint64_t amount,
                  string memo);

    ACTION create(name org,
                  name antibadge,
                  name badge,
                  name type,
                  string offchain_lookup_data,
                  string onchain_lookup_data,
                  string memo);

    ACTION createinv(name org,
                     name antibadge,
                     name badge,
                     string offchain_lookup_data,
                     string onchain_lookup_data,
                     string memo);

    ACTION issue(name org,
                 name to,
                 name antibadge,
                 uint64_t amount,
                 string memo);

  private:
    struct create_args {
        name org;
        name antibadge;
        name badge;
        name type;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct createinv_args {
        name org;
        name antibadge;
        name badge;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct issue_args {
        name org;
        name to;
        name antibadge;
        uint64_t amount;
        string memo;
    };
};
