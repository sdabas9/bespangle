#include <antibadge.hpp>

ACTION antibadge::create(symbol anti_badge_symbol,
                         symbol badge_symbol,
                         name type,
                         string offchain_lookup_data,
                         string onchain_lookup_data,
                         string memo) {

    string action_name = "create";
    string failure_identifier = "CONTRACT: antibadge, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);  

    auto _onchain_lookup_data = json::parse(onchain_lookup_data);

    _onchain_lookup_data["anti_to"] = badge_symbol.code().to_string(); 
    
    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("initbadge"),
      initbadge_args {
        .badge_symbol = anti_badge_symbol,
        .offchain_lookup_data = offchain_lookup_data,
        .onchain_lookup_data = _onchain_lookup_data.dump(),
        .memo = memo }
    }.send();

    name org = get_org_from_badge_symbol(badge_symbol, failure_identifier);
    json antibadge1;
    antibadge1["type"] = type.to_string();
    antibadge1["badge"] = anti_badge_symbol.code().to_string();
    json antibadge_array = anti_badges(org, badge_symbol);
    antibadge_array.push_back(antibadge1);
    json badge_onchain_lookup_data;
    badge_onchain_lookup_data["antibadge"] = antibadge_array;
    
    action{
        permission_level{get_self(), name("active")},
        name(ORCHESTRATOR_CONTRACT),
        name("mergeinfo"),
        mergeinfo_args{
            .badge_symbol = badge_symbol,
            .offchain_lookup_data = "",
            .onchain_lookup_data = badge_onchain_lookup_data.dump(),
            .memo = memo
        }
    }.send();
}

ACTION antibadge::createinv (symbol anti_badge_symbol,
    symbol badge_symbol,
    string offchain_lookup_data,
    string onchain_lookup_data,
    string memo) {
    
    string action_name = "createinv";
    string failure_identifier = "CONTRACT: antibadge, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);  

    action{
        permission_level{get_self(), name("active")},
        name(get_self()),
        name("create"),
        create_args{
            .anti_badge_symbol = anti_badge_symbol,
            .badge_symbol = badge_symbol,
            .type = name("invalidate"),
            .offchain_lookup_data = offchain_lookup_data,
            .onchain_lookup_data = onchain_lookup_data,
            .memo = memo
        }
    }.send();
}

ACTION antibadge::issue(asset anti_badge_asset, name to, string memo) {
    string action_name = "issue";
    string failure_identifier = "CONTRACT: antibadge, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier); 

    name org = get_org_from_badge_symbol(anti_badge_asset.symbol, failure_identifier);
    
    badge_table badge(name(ORCHESTRATOR_CONTRACT), org.value);
    auto badge_itr = badge.require_find(anti_badge_asset.symbol.code().raw(), "antibadge not defined. create badge before issuing");
    vector<name> notify_accounts = badge_itr->notify_accounts;
    for(auto i = 0 ; i < notify_accounts.size(); i++) {
        if(notify_accounts[i] == name(CUMULATIVE_CONTRACT)) {
            action{
                permission_level{get_self(), name("active")},
                get_self(),
                name("chkissue"),
                chkissue_args {
                    .badge_asset = anti_badge_asset,
                    .account = to
                }
            }.send();
        }
        if(notify_accounts[i] == name(BOUNDED_AGG_CONTRACT)) {
            action{
                permission_level{get_self(), name("active")},
                get_self(),
                name("chkbndissue"),
                chkissue_args {
                    .badge_asset = anti_badge_asset,
                    .account = to
                }
            }.send();
        }
    }


    action {
      permission_level{get_self(), name("active")},
      name(ORCHESTRATOR_CONTRACT),
      name("achievement"),
      achievement_args {
        .badge_asset = anti_badge_asset,
        .from = get_self(),
        .to = to,
        .memo = memo }
    }.send();
}

ACTION antibadge::chkissue (asset anti_badge_asset, name account) {
    string action_name = "chkissue";
    string failure_identifier = "CONTRACT: antibadge, ACTION: " + action_name + ", MESSAGE: ";
    
    name org = get_org_from_badge_symbol(anti_badge_asset.symbol, failure_identifier);
    symbol badge_symbol = get_badge_for_antibadge(org, anti_badge_asset.symbol);
    vector<symbol> anti_badge_symbols = anti_badge_vector(org, badge_symbol);
    uint64_t total_antibadge_balance = multiple_badges_additive_balance(anti_badge_symbols, account);
    uint64_t badge_balance = account_balance(badge_symbol, account);
    check(total_antibadge_balance + anti_badge_asset.amount <= badge_balance, "CHECK FAILED: cumulative balance of all antibadges will exceed the badge balance");
}

ACTION antibadge::chkbndissue (asset anti_badge_asset, name account) {
    string action_name = "chkbndissue";
    string failure_identifier = "CONTRACT: antibadge, ACTION: " + action_name + ", MESSAGE: ";

    name org = get_org_from_badge_symbol(anti_badge_asset.symbol, failure_identifier);
    symbol badge_symbol = get_badge_for_antibadge(org, anti_badge_asset.symbol);
    vector<symbol> anti_badge_symbols = anti_badge_vector(org, badge_symbol);

    achievements_table achievements(name(BOUNDED_AGG_CONTRACT), account.value);

    badgestatus_table badgestatus(name(BOUNDED_AGG_CONTRACT), org.value);
    auto by_status_index = badgestatus.get_index<"bybadgestat"_n>();
    auto hashed_active_status = hash_active_status(anti_badge_asset.symbol, "active"_n, "active"_n);
    auto itr = by_status_index.find(hashed_active_status);
    
    while(itr != by_status_index.end() && itr->badge_symbol == anti_badge_asset.symbol && itr->badge_status == "active"_n && itr->seq_status == "active"_n) {
        symbol agg_symbol = itr->agg_symbol;
        uint64_t seq_id = itr->seq_id;
        uint64_t badge_balance = 0;
        uint64_t anti_badge_balance = 0;
        
        auto by_agg_seq_badge_index = badgestatus.get_index<"aggseqbadge"_n>();

        auto badge_hashed_key = hash_agg_seq_badge(itr->agg_symbol, itr->seq_id, badge_symbol);
        auto badge_itr = by_agg_seq_badge_index.find(badge_hashed_key);

        if( badge_itr != by_agg_seq_badge_index.end() && 
            agg_symbol == badge_itr->agg_symbol && 
            seq_id == badge_itr->seq_id && 
            badge_symbol == badge_itr->badge_symbol) {
            auto ach_itr = achievements.find(badge_itr->badge_agg_seq_id);
            badge_balance = (ach_itr == achievements.end()) ? 0 : ach_itr->count;
        }
        for(auto i = 0 ; i < anti_badge_symbols.size(); i++) {
            auto antibadge_hashed_key = hash_agg_seq_badge(itr->agg_symbol, itr->seq_id, anti_badge_symbols[i]);
            auto anti_badge_itr = by_agg_seq_badge_index.find(antibadge_hashed_key);
            if( anti_badge_itr != by_agg_seq_badge_index.end() && 
                agg_symbol == anti_badge_itr->agg_symbol && 
                seq_id == anti_badge_itr->seq_id && 
                anti_badge_symbols[i] == anti_badge_itr->badge_symbol) {

                auto ach_itr = achievements.find(anti_badge_itr->badge_agg_seq_id);
                anti_badge_balance = anti_badge_balance + (ach_itr == achievements.end()) ? 0 : ach_itr->count;
            }

        }
        
        check(anti_badge_balance + anti_badge_asset.amount <= badge_balance, "CHECK FAILED: cumulative balance of all antibadges will exceed the badge balance for agg : " + agg_symbol.code().to_string() + " seq id:" + std::to_string(seq_id));
        itr++;
    }


}
