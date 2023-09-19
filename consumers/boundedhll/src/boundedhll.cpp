#include <boundedhll.hpp>

void boundedhll::notifyachiev(
    name org, 
    name badge,
    name account, 
    name from,
    uint64_t count,
    string memo,  
    vector<name> notify_accounts
) {
    badgeround_table _badgerounds(_self, org.value);
    auto badge_index = _badgerounds.get_index<"badge"_n>();
    auto badge_itr = badge_index.lower_bound(badge.value);

    check(badge_itr != badge_index.end() && badge_itr->badge == badge, "Badge not found for any round");

    while (badge_itr != badge_index.end() && badge_itr->badge == badge) {
        rounds_table _rounds(_self, org.value);
        auto round_itr = _rounds.find(badge_itr->round.value);
        check(round_itr != _rounds.end(), "Round not found");

        if (round_itr->status == name("ongoing")) {
            name emit_badge = badge_itr->sender_uniqueness_badge;

            // Create an instance of the balances_table scoped by the organization
            balances_table balances(_self, org.value);

            // Generate composite key: combination of account and badge
            uint128_t composite_key = (static_cast<uint128_t>(account.value) << 64) | badge_itr->badgeround_id;

            // Find an existing record by composite key
            auto secondary_index = balances.get_index<"accountbadge"_n>();
            auto existing_entry = secondary_index.find(composite_key);

            uint8_t b = 7;
            uint32_t m = 1 << b;

            if (existing_entry == secondary_index.end()) {
                // Initialize HyperLogLog data structure
                vector<uint8_t> M(m, 0);
                hll::HyperLogLog hll(b, m, M);
                hll.add(from.to_string().c_str(), from.to_string().size());

                // No existing entry found, insert a new record
                balances.emplace(org, [&](auto& row) {
                    row.id = balances.available_primary_key();
                    row.account = account;
                    row.badgeround_id = badge_itr->badgeround_id;
                    row.hll = hll.registers();
                });
            } else if (existing_entry != secondary_index.end() &&
                       existing_entry->account == account && 
                       existing_entry->badgeround_id == badge_itr->badgeround_id) {
                // Check for increment condition
                bool increment = false;
                
                // Get existing HyperLogLog data
                vector<uint8_t> M = existing_entry->hll;
                hll::HyperLogLog hll(b, m, M);
                
                // Estimate cardinality before and after adding new element
                double estimate_before = hll.estimate();
                hll.add(from.to_string().c_str(), from.to_string().size());
                double estimate_after = hll.estimate();

                // Check if cardinality has increased
                if (estimate_before != estimate_after) {
                    secondary_index.modify(existing_entry, get_self(), [&](auto& row) {
                        row.hll = hll.registers();
                    });

                    action {
                        permission_level{get_self(), name("active")},
                        name(get_self()),
                        name("issuesimple"),
                        issue_args {
                            .org = org,
                            .to = account,
                            .badge = emit_badge,
                            .amount = 1,
                            .memo = "issued from rollup consumer"
                        }
                    }.send();
                }
            }
        }

        badge_itr++;
    }
}



ACTION boundedhll::addround(name org, name round, string description) {
    require_auth(_self);
    check(description.size() <= 40, "Description can only be up to 40 characters long");
    
    rounds_table _rounds(_self, org.value);
    auto round_itr = _rounds.find(round.value);
    check(round_itr == _rounds.end(), "Round already exists");

    _rounds.emplace(_self, [&](auto& new_round) {
        new_round.round = round;
        new_round.status = "init"_n;
        new_round.description = description;
    });
}

ACTION boundedhll::setstatus(name org, name round, name status) {
    require_auth(_self);

    rounds_table _rounds(_self, org.value);
    auto round_itr = _rounds.find(round.value);
    check(round_itr != _rounds.end(), "Round not found");

    check(
        (round_itr->status == "init"_n && status == "ongoing"_n) || 
        (round_itr->status == "ongoing"_n && status == "end"_n), 
        "Invalid status transition"
    );

    _rounds.modify(round_itr, _self, [&](auto& update_round) {
        update_round.status = status;
    });
}

ACTION boundedhll::newemission(
    name org,
    name round,
    name badge,
    name sender_uniqueness_badge
) {
    require_auth(_self);

    rounds_table _rounds(_self, org.value);
    auto round_itr = _rounds.find(round.value);
    check(round_itr != _rounds.end(), "Round not found");
    check(round_itr->status == "init"_n, "Can only add badges to rounds in 'init' status");

    badgeround_table _badgerounds(_self, org.value);
    auto badge_index = _badgerounds.get_index<"roundbadge"_n>();
    uint128_t composite_key = ((uint128_t) round.value) << 64 | badge.value;
    check(badge_index.find(composite_key) == badge_index.end(), "Badge already added to this round");

    _badgerounds.emplace(_self, [&](auto& new_badgeround) {
        new_badgeround.badgeround_id = _badgerounds.available_primary_key();
        new_badgeround.round = round;
        new_badgeround.badge = badge;
        new_badgeround.sender_uniqueness_badge = sender_uniqueness_badge;
    });
}

ACTION boundedhll::issuesimple(name org, name to, name badge, uint64_t amount, string memo) {
    require_auth(get_self());
    require_recipient(name(NOTIFICATION_CONTRACT_NAME));
    // Implementation here
}
