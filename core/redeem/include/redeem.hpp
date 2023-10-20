#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <json.hpp>
using json = nlohmann::json;

using namespace std;
using namespace eosio;
using eosio::singleton;

#define NOTIFICATION_CONTRACT_NAME "notifica"
#define ORCHESTRATOR_CONTRACT_NAME "orc"
#define BOUNDEDAGG_CONTRACT_NAME "badg"

CONTRACT redeem : public contract {
  public:
    using contract::contract;


ACTION initfixed(name org, name round, name badge, uint64_t rate, name notification_contract);

ACTION initdynamic(name org, name round, name badge, uint64_t amount, name notification_contract);

ACTION redeemfix(name org, name round, name badge, name account);

ACTION redeemdyn(name org, name round, name badge, name account);

ACTION distfixed(name org, name round, name badge, name account, uint64_t amount_val, name notification_contract) {

ACTION distdyanmic(name org, name round, name badge, name account, uint64_t portion, name notification_contract) {


ACTION issuesbt(name org,
    name to,
    name antibadge,
    uint64_t amount,
    string memo);

struct issuesbt_args {
    name org;
    name to;
    name antibadge;
    uint64_t amount;
    string memo;
};

struct transfer_args {
    name from;
    name to;
    asset quantity;
    string memo;
};

struct initfixed_args {
    name org;
    name round;
    name badge;
    uint64_t rate; 
    name notification_contract;
};

struct initdynamic_args {
    name org;
    name round;
    name badge;
    uint64_t amount;
    name notification_contract;
};
struct distfixed_args {
    name org;
    name round;
    name badge;
    name account;
    uint64_t amount_val;
    name notification_contract;
};

struct distdynamic_args {
    name org;
    name round;
    name badge;
    name account;
    uint64_t portion;
    name notification_contract;
};

  private:

    TABLE fixed {
        uint64_t id;
        name round;
        name badge;
        name contract;
        uint64_t rate;
        uint64_t primary_key() const { return id; }
        uint128_t by_round_and_badge() const { return (uint128_t(round.value) << 64) | badge.value; }
    };
    typedef multi_index<name("fixed"), fixed,
        indexed_by<name("byroundbadge"), const_mem_fun<fixed, uint128_t, &fixed::by_round_and_badge>>
    > fixed_table;

    TABLE dynamic {
        uint64_t id;
        name round;
        name badge;
        name contract;
        uint64_t amount;
        uint64_t primary_key() const { return id; }
        uint128_t by_round_and_badge() const { return (uint128_t(round.value) << 64) | badge.value; }
    };
    typedef multi_index<name("dynamic"), dynamic,
        indexed_by<name("byroundbadge"), const_mem_fun<dynamic, uint128_t, &dynamic::by_round_and_badge>>
    > dynamic_table;

    TABLE ids {
        uint64_t id;
    };
    typedef singleton<name("ids"), ids> ids_table;

    uint64_t get_next_id() {
    ids_table id_t(get_self(), get_self().value);
    auto id_obj = id_t.get_or_create(get_self(), ids{0});
    id_t.set(ids{id_obj.id + 1}, get_self());
    return id_obj.id + 1;
}

TABLE keystats {
    uint64_t id;
    name round;
    name badge;
    uint64_t max;
    uint64_t mean;
    uint64_t account_count;
    uint64_t total; 
    auto primary_key() const { return id; }
    uint128_t by_round_and_badge() const { return (uint128_t(round.value) << 64) | badge.value; }
};
typedef multi_index<name("keystats"), keystats,
    indexed_by<name("byroundbadge"), const_mem_fun<keystats, uint128_t, &keystats::by_round_and_badge>>
> keystats_table;

TABLE badgeround {
    uint64_t badgeround_id;
    name round;
    name badge;
    name status;
    auto primary_key() const { return badgeround_id; }
    uint128_t round_badge_key() const { return ((uint128_t) round.value) << 64 | badge.value; }
    uint64_t get_badge() const { return badge.value; }
};
typedef multi_index<"badgeround"_n, badgeround,
    indexed_by<"roundbadge"_n, const_mem_fun<badgeround, uint128_t, &badgeround::round_badge_key>>,
    indexed_by<"badge"_n, const_mem_fun<badgeround, uint64_t, &badgeround::get_badge>>
> badgeround_table;


TABLE achievements {
    uint64_t id;
    name account;
    uint64_t badgeround_id;
    uint64_t count;
    auto primary_key() const {return id; }
    uint128_t acc_badge_key() const { return ((uint128_t) account.value) << 64 | badgeround_id; }
};
typedef multi_index<"achievements"_n, achievements,
    indexed_by<"accbadge"_n, const_mem_fun<achievements, uint128_t, &achievements::acc_badge_key>>
> achievements_table;

TABLE mbadge {
    name badge;
    vector<name> notify_accounts;
    string offchain_lookup_data;
    string onchain_lookup_data;
    uint64_t rarity_counts;
    auto primary_key() const {return badge.value; }
};
typedef multi_index<name("badge"), mbadge> metadata_badge_table;

vector<name> anti_badge_vector (name org, name badge) {
    metadata_badge_table _badge(name(ORCHESTRATOR_CONTRACT_NAME), org.value);
    auto badge_iterator = _badge.find (badge.value);
    check(badge_iterator == _badge.end(), "UNEXPECTED: badge not created"); 
    auto onchain_lookup_data = json::parse(badge_iterator->onchain_lookup_data);
    json entries = onchain_lookup_data["antibadge"];
    vector<name> anti_badges;
    for (const auto& entry : entries) {
    string badge_name = entry["badge"];
    anti_badges.push_back(name(badge_name));
    }
    return anti_badges;
}

name redeem_badge (name org, name badge) {
    metadata_badge_table _badge(name(ORCHESTRATOR_CONTRACT_NAME), org.value);
    auto badge_iterator = _badge.find (badge.value);
    check(badge_iterator == _badge.end(), "UNEXPECTED: badge not created"); 
    auto onchain_lookup_data = json::parse(badge_iterator->onchain_lookup_data);
    json entries = onchain_lookup_data["antibadge"];
    bool found = false;
    string redeem_badge_name;
    for (const auto& entry : entries) {
        if (entry["type"] == name("redeem")) {
            redeem_badge_name = entry["badge"];
            found = true;
        }
    }
    check(found, "No redeem badge for badge <badge>");
    return name(redeem_badge_name);
}

    uint64_t bounded_account_balance(name org, name round, name badge, name account) {
        uint64_t badgeround_id = get_badgeround_id(org, round, badge);
        uint64_t balance = bounded_account_balance_for_badgeround_id(org, badgeround_id, account);
        return balance;
    }

    uint64_t bounded_account_balance_for_badgeround_id(name org, uint64_t badgeround_id, name account) {
        achievements_table _achievements(name(BOUNDEDAGG_CONTRACT_NAME), org.value);
        auto account_badge_index = _achievements.get_index<"accbadge"_n>();

        uint128_t key = ((uint128_t) account.value) << 64 | badgeround_id;
        auto account_badge_iterator = account_badge_index.find(key);

        if (account_badge_iterator == account_badge_index.end() ||
            account_badge_iterator->account != account ||
            account_badge_iterator->badgeround_id != badgeround_id) {
            return 0;
        } else {
            return account_badge_iterator->count;
        }
    }

    uint64_t get_badgeround_id(name org, name round, name badge) {
        badgeround_table _badgeround(name(BOUNDEDAGG_CONTRACT_NAME), org.value);
        auto round_badge_index = _badgeround.get_index<"roundbadge"_n>();
        uint128_t round_badge_key = ((uint128_t) round.value) << 64 | badge.value;
        auto round_badge_iterator = round_badge_index.find(round_badge_key);

        if (round_badge_iterator != round_badge_index.end() &&
            round_badge_iterator->round == round &&
            round_badge_iterator->badge == badge) {
            return round_badge_iterator->badgeround_id;
        }
        check(false, "unexpected error in checks contract");
    }

    uint64_t bounded_multiple_badges_additive_balance(name org, vector<uint64_t> badgeround_ids, name account) {
        uint64_t balance = 0;
        for (auto i = 0; i < badgeround_ids.size(); i++) {
            balance += bounded_account_balance_for_badgeround_id(org, badgeround_ids[i], account);
        }
        return balance;
    }

    vector<uint64_t> relevant_badgeround_ids(name org, name round, vector<name> antibadges) {
        badgeround_table _badgerounds(name(BOUNDEDAGG_CONTRACT_NAME), org.value);
        vector<uint64_t> badgeround_ids;

        for (auto i = 0; i < antibadges.size(); i++) {
            auto round_badge_index = _badgerounds.get_index<"roundbadge"_n>();
            uint128_t round_badge_key = ((uint128_t) round.value) << 64 | antibadges[i].value;
            auto round_badge_iterator = round_badge_index.find(round_badge_key);

            if (round_badge_iterator != round_badge_index.end() &&
                round_badge_iterator->round == round &&
                round_badge_iterator->badge == antibadges[i] &&
                round_badge_iterator->status == name("active")) {
                badgeround_ids.push_back(round_badge_iterator->badgeround_id);
            }
        }
        return badgeround_ids;
    }

};
