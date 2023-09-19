#include <antibadge.hpp>
#include <json.hpp>
using json = nlohmann::json;

void antibadge::extcreate(name org,
               name antibadge,
               name badge,
               name type,
               string offchain_lookup_data,
               string onchain_lookup_data,
               string memo) {
    action{
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("create"),
        create_args{
            .org = org,
            .antibadge = antibadge,
            .badge = badge,
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = onchain_lookup_data,
            .memo = memo
        }
    }.send();
}

void antibadge::extcinv(name org,
             name antibadge,
             name badge,
             string offchain_lookup_data,
             string onchain_lookup_data,
             string memo) {
    action{
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("createinv"),
        createinv_args{
            .org = org,
            .antibadge = antibadge,
            .badge = badge,
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = onchain_lookup_data,
            .memo = memo
        }
    }.send();
}

void antibadge::extissue(name org,
              name to,
              name antibadge,
              uint64_t amount,
              string memo) {
    action{
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("issue"),
        issue_args{
            .org = org,
            .to = to,
            .antibadge = antibadge,
            .amount = amount,
            .memo = memo
        }
    }.send();
}

ACTION antibadge::create(name org,
                         name antibadge,
                         name badge,
                         name type,
                         string offchain_lookup_data,
                         string onchain_lookup_data,
                         string memo) {
    require_auth(get_self());
    auto _onchain_lookup_data = json::parse(onchain_lookup_data);

    _onchain_lookup_data["anti_to"] = badge.to_string();

    action{
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT_NAME),
        name("initbadge"),
        initbadge_args{
            .org = org,
            .badge_name = antibadge,
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = _onchain_lookup_data.dump(),
            .memo = memo
        }
    }.send();

    json antibadge1;
    antibadge1["type"] = type.to_string();
    antibadge1["badge"] = antibadge.to_string();
    json antibadge_array = json::array();
    antibadge_array.push_back(antibadge1);
    json badge_onchain_lookup_data;
    badge_onchain_lookup_data["antibadge"] = antibadge_array;

    action{
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT_NAME),
        name("mergeinfo"),
        mergeinfo_args{
            .org = org,
            .badge = badge,
            .offchain_lookup_data = "",
            .onchain_lookup_data = badge_onchain_lookup_data.dump(),
            .memo = memo
        }
    }.send();
}

ACTION antibadge::createinv(name org,
                            name antibadge,
                            name badge,
                            string offchain_lookup_data,
                            string onchain_lookup_data,
                            string memo) {
    require_auth(get_self());

    action{
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("create"),
        create_args{
            .org = org,
            .antibadge = antibadge,
            .badge = badge,
            .type = name("invalidate"),
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = onchain_lookup_data,
            .memo = memo
        }
    }.send();
}
// m1 .. mn
// p, p, p, p, p
// r, r, r, r, r

ACTION antibadge::issue(name org,
                        name to,
                        name antibadge,
                        uint64_t amount,
                        string memo) {
    require_auth(get_self());
    
    badge_table badge(name(ORCHESTRATOR_CONTRACT_NAME), org.value);
    auto badge_itr = badge.require_find(antibadge.value, "antibadge not defined. create badge before issuing");
    vector<name> notify_accounts = badge_itr->notify_accounts;
    for(auto i = 0 ; i < notify_accounts.size(); i++) {
        if(notify_accounts[i] == name(CUMULATIVE_CONTRACT_NAME)) {
            action{
                permission_level{get_self(), name("active")},
                name(CHECKS_CONTRACT_NAME),
                name("canissueanti"),
                canissueanti_args{
                    .org = org,
                    .account = to,
                    .antibadge = antibadge,
                    .amount = amount
                }
            }.send();
        }
        if(notify_accounts[i] == name(BOUNDEDAGG_CONTRACT_NAME)) {
            action{
                permission_level{get_self(), name("active")},
                name(CHECKS_CONTRACT_NAME),
                name("issuebndanti"),
                canissueanti_args{
                    .org = org,
                    .account = to,
                    .antibadge = antibadge,
                    .amount = amount
                }
            }.send();
        }
    }


    action{
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT_NAME),
        name("achievement"),
        achievement_args{
            .org = org,
            .badge_name = antibadge,
            .account = to,
            .from = org,
            .count = amount,
            .memo = memo
        }
    }.send();
}
