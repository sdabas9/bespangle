#include <boundedhll.hpp>

void boundedhll::notifyachiev (
    name org,
    asset badge_asset,
    name from,
    name to,
    string memo,
    vector<name> notify_accounts
) {
    string action_name = "notifyachiev";
    string failure_identifier = "CONTRACT: boundedhll, ACTION: " + action_name + ", MESSAGE: ";
    badgestatus_table badgestatus(name(BOUNDED_AGG_CONTRACT), org.value);
    auto badge_status_index = badgestatus.get_index<"bybadgestat"_n>();
    auto hashed_active_status = hash_active_status(badge_asset.symbol, "active"_n, "active"_n);
    auto itr = badge_status_index.find(hashed_active_status);

    while(itr != badge_status_index.end() && itr->badge_symbol == badge_asset.symbol && itr->badge_status == "active"_n && itr->seq_status == "active"_n) {
        emissions_table emissions(_self, itr->agg_symbol.code().raw());
        auto emissions_itr = emissions.find(itr->badge_symbol.code().raw());

        if(emissions_itr != emissions.end() && emissions_itr->status == name("active")) {
            balances_table _balances(_self, to.value);
            auto balances_itr = _balances.find(itr->badge_agg_seq_id);

            uint8_t b = 7;
            uint32_t m = 1 << b;

            if (balances_itr == _balances.end()) {
                vector<uint8_t> M(m, 0);
                hll::HyperLogLog hll(b, m, M);
                hll.add(from.to_string().c_str(), from.to_string().size());

                _balances.emplace(get_self(), [&](auto& row) {
                    row.badge_agg_seq_id = itr->badge_agg_seq_id;
                    row.hll = hll.registers();
                });

                for(auto i = 0 ; i < emissions_itr->sender_uniqueness_badge_symbols.size(); i++) {
                    action {
                        permission_level{get_self(), name("active")},
                        name(name(SIMPLEBADGE_CONTRACT)),
                        name("issue"),
                        issue_args {
                            .org = org,
                            .badge_asset = eosio::asset(1, emissions_itr->sender_uniqueness_badge_symbols[i]),
                            .to = to,
                            .memo = "issued from rollup consumer"
                        }
                    }.send();
                }
            } else {
                vector<uint8_t> M = balances_itr->hll;
                hll::HyperLogLog hll(b, m, M);
                double estimate_before = hll.estimate();
                hll.add(from.to_string().c_str(), from.to_string().size());
                double estimate_after = hll.estimate();

                if (estimate_before != estimate_after) {
                    _balances.modify(balances_itr, get_self(), [&](auto& row) {
                        row.hll = hll.registers();
                    });
                    for(auto i = 0 ; i < emissions_itr->sender_uniqueness_badge_symbols.size(); i++) {
                        action {
                            permission_level{get_self(), name("active")},
                            name(name(SIMPLEBADGE_CONTRACT)),
                            name("issue"),
                            issue_args {
                                .org = org,
                                .badge_asset = eosio::asset(1, emissions_itr->sender_uniqueness_badge_symbols[i]),
                                .to = to,
                                .memo = "issued from rollup consumer"
                            }
                        }.send();
                    }
                }
            }
        }
        itr++;
    }
}

ACTION boundedhll::newemission (
    name org, 
    symbol agg_symbol,
    symbol badge_symbol,
    vector<symbol> sender_uniqueness_badge_symbols
) {
    string action_name = "newemission";
    string failure_identifier = "CONTRACT: boundedhll, ACTION: " + action_name + ", MESSAGE: ";    
    check_internal_auth(name(action_name), failure_identifier);
    
    emissions_table emissions(_self, agg_symbol.code().raw());
    auto emissions_itr = emissions.find(badge_symbol.code().raw());
    check(emissions_itr == emissions.end(), failure_identifier + "emission already exist");

    emissions.emplace(get_self(), [&](auto& row) {
        row.badge_symbol = badge_symbol;
        row.status = name("init");
        row.sender_uniqueness_badge_symbols = sender_uniqueness_badge_symbols;
    });

    action {
        permission_level{get_self(), name("active")},
        name(SUBSCRIPTION_CONTRACT),
        name("billing"),
        billing_args {
            .org = org,
            .actions_used = 1}
    }.send();
}

ACTION boundedhll::activate(name org, symbol agg_symbol, symbol badge_symbol) {
    string action_name = "activate";
    string failure_identifier = "CONTRACT: boundedhll, ACTION: " + action_name + ", MESSAGE: ";    
    check_internal_auth(name(action_name), failure_identifier);
    
    emissions_table emissions(_self, agg_symbol.code().raw());
    auto emissions_itr = emissions.find(badge_symbol.code().raw());
    check(emissions_itr != emissions.end(), failure_identifier + " emission does not exist for agg, seq, badge");

    emissions.modify(emissions_itr, get_self(), [&](auto& row) {
        row.status = name("active");
    });


}

ACTION boundedhll::deactivate(name org, symbol agg_symbol, symbol badge_symbol) {
    string action_name = "deactivate";
    string failure_identifier = "CONTRACT: boundedhll, ACTION: " + action_name + ", MESSAGE: ";    
    check_internal_auth(name(action_name), failure_identifier);
    
    emissions_table emissions(_self, agg_symbol.code().raw());
    auto emissions_itr = emissions.find(badge_symbol.code().raw());
    check(emissions_itr != emissions.end(), failure_identifier + " emission does not exist for agg, seq, badge");

    emissions.modify(emissions_itr, get_self(), [&](auto& row) {
        row.status = name("inactive");
    });

}
