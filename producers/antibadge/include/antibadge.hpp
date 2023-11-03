#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define ORCHESTRATOR_CONTRACT_NAME "router111111"
#define CHECKS_CONTRACT_NAME "checks111111"
#define NOTIFICATION_CONTRACT "sdsdf"
#define BOUNDEDAGG_CONTRACT_NAME "dsfd"

#define CREATE_NOTIFICATION NOTIFICATION_CONTRACT "::initsimple"
#define CREATE_INVALIDATE_NOTIFICATION NOTIFICATION_CONTRACT "::initsimple"
#define ISSUANCE_NOTIFICATION NOTIFICATION_CONTRACT "::givesimple"

CONTRACT antibadge : public contract {
public:
    using contract::contract;

    [[eosio::on_notify(CREATE_NOTIFICATION)]]
    void extcreate(name org,
                   name antibadge,
                   name badge,
                   name type,
                   string offchain_lookup_data,
                   string onchain_lookup_data,
                   string memo);

    [[eosio::on_notify(CREATE_INVALIDATE_NOTIFICATION)]]
    void extcinv(name org,
                 name antibadge,
                 name badge,
                 string offchain_lookup_data,
                 string onchain_lookup_data,
                 string memo);

    [[eosio::on_notify(ISSUANCE_NOTIFICATION)]]
    void extissue(name org,
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
    TABLE badge {
      name badge;
      vector<name> notify_accounts;
      string offchain_lookup_data;
      string onchain_lookup_data;
      uint64_t rarity_counts;
      auto primary_key() const {return badge.value; }
    };
    typedef multi_index<name("badge"), badge> badge_table;


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

    struct achievement_args {
        name org;
        name badge_name;
        name account;
        name from;
        uint64_t count;
        string memo;
    };

    struct initbadge_args {
        name org;
        name badge_name;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct mergeinfo_args {
        name org;
        name badge;
        string offchain_lookup_data;
        string onchain_lookup_data;
        string memo;
    };

    struct canissueanti_args {
        name org;
        name account;
        name antibadge;
        uint64_t amount;
    };
};