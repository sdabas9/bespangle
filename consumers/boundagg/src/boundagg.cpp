#include <boundagg.hpp>

ACTION boundagg::addrnd(name org, name round, string description) {
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

ACTION boundagg::setstat(name org, name round, name status) {
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

ACTION boundagg::addbgrnd(name org, name round, name badge) {
    require_auth(_self);

    rounds_table _rounds(_self, org.value);
    auto round_itr = _rounds.find(round.value);
    check(round_itr != _rounds.end(), "Round not found");
    check(round_itr->status == "init"_n || round_itr->status == "ongoing"_n, "Can only add badges to rounds in 'init' or 'ongoing' status");

    badgeround_table _badgerounds(_self, org.value);
    auto badge_index = _badgerounds.get_index<"roundbadge"_n>();
    uint128_t composite_key = ((uint128_t) round.value) << 64 | badge.value;
    auto badge_iterator = badge_index.find(composite_key);
    if(badge_iterator == badge_index.end() || 
        badge_iterator->round!=round || 
        badge_iterator->badge!=badge) {
        _badgerounds.emplace(_self, [&](auto& new_badgeround) {
            new_badgeround.badgeround_id = _badgerounds.available_primary_key();
            new_badgeround.round = round;
            new_badgeround.badge = badge;
            new_badgeround.status = name("active");
        });
    } else if(badge_iterator != badge_index.end() && 
        badge_iterator->round ==round &&
        badge_iterator->badge == badge) {
        
        check(badge_iterator->status != name("active"), "Badge already added to the round and is in active state");
        badge_index.modify(badge_iterator, get_self(), [&](auto& row) {
            row.status = name("active");
        });
    }
    
}

ACTION boundagg::rembgrnd(name org, name round, name badge) {
    require_auth(_self);

    rounds_table _rounds(_self, org.value);
    auto round_itr = _rounds.find(round.value);
    check(round_itr != _rounds.end(), "Round not found");
    check(round_itr->status == "init"_n || round_itr->status == "ongoing"_n, "Can only add badges to rounds in 'init' or 'ongoing' status");

    badgeround_table _badgerounds(_self, org.value);
    auto badge_index = _badgerounds.get_index<"roundbadge"_n>();
    uint128_t composite_key = ((uint128_t) round.value) << 64 | badge.value;
    auto badge_iterator = badge_index.find(composite_key);

    check(badge_iterator != badge_index.end() &&
        badge_iterator->round == round &&
        badge_iterator->badge == badge, "add badge to the round first");
    check(badge_iterator->status != name("inactive"), "status already inactive");

    badge_index.modify(badge_iterator, get_self(), [&](auto& row) {
        row.status = name("inactive");
    });
 
}

void boundagg::notifyachiev(
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

    if(badge_itr == badge_index.end() || badge_itr->badge != badge) {
        return;
    }

    do {
        if(badge_itr->status == name("active")) {
            // Ensure the associated round is in the "ongoing" status
            rounds_table _rounds(_self, org.value);
            auto round_itr = _rounds.find(badge_itr->round.value);
            check(round_itr != _rounds.end(), "Round not found");
            
            if (round_itr->status == "ongoing"_n) {
                // Update the achievements table for this round
                achievements_table _achievements(_self, org.value);
                auto acc_badge_index = _achievements.get_index<"accbadge"_n>();
                uint128_t composite_key = ((uint128_t) account.value) << 64 | badge_itr->badgeround_id;
                auto achievement_itr = acc_badge_index.find(composite_key);

                if (achievement_itr == acc_badge_index.end()) {
                    // Create a new record if not found
                    _achievements.emplace(_self, [&](auto& row) {
                        row.id = _achievements.available_primary_key();
                        row.account = account;
                        row.badgeround_id = badge_itr->badgeround_id;
                        row.count = count;
                    });
                } else {
                    // Update the existing record
                    acc_badge_index.modify(achievement_itr, _self, [&](auto& row) {
                        row.count += count;
                    });
                }
            }

        }

        // Move to the next round associated with the same badge
        badge_itr++;

    } while (badge_itr != badge_index.end() && badge_itr->badge == badge);
}
