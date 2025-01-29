#include <metadata.hpp>
#include <json.hpp>
using json = nlohmann::json;

ACTION metadata::initbadge(
    name org,
    symbol badge_symbol, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo) {

    string action_name = "initbadge";
    string failure_identifier = "CONTRACT: metadata, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    badge_table badges(get_self(), org.value);

    // Check if the badge with the given symbol already exists
    auto badge_itr = badges.find(badge_symbol.code().raw());
    check(badge_itr == badges.end(), failure_identifier + "Badge with this symbol already exists");

    // Add new badge record to the table
    badges.emplace(get_self(), [&](auto& new_badge) {
      new_badge.badge_symbol = badge_symbol;
      new_badge.offchain_lookup_data = offchain_lookup_data;
      new_badge.onchain_lookup_data = onchain_lookup_data;
    });
}

ACTION metadata::addfeature(
    name org, 
    symbol badge_symbol, 
    name notify_account, 
    string memo) {

    string action_name = "addfeature";
    string failure_identifier = "CONTRACT: metadata, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    badge_table _badge(get_self(), org.value);

    auto badge_iterator = _badge.find(badge_symbol.code().raw());
    check(badge_iterator != _badge.end(), failure_identifier + " badge_symbol not initialized ");

    vector<name> new_notify_accounts;
    for (auto i = 0; i < badge_iterator->notify_accounts.size(); i++) {
        if (notify_account == badge_iterator->notify_accounts[i]) {
            return;
        }
        new_notify_accounts.push_back(badge_iterator->notify_accounts[i]);
    }
    new_notify_accounts.push_back(notify_account);
    _badge.modify(badge_iterator, get_self(), [&](auto& row) {
        row.notify_accounts = new_notify_accounts;
    });

    action {
        permission_level{get_self(), name("active")},
        get_self(),
        name("addnotify"),
        downstream_notify_args {
            .org = org,
            .badge_symbol = badge_symbol,
            .notify_account = notify_account,
            .memo = memo,
            .offchain_lookup_data = badge_iterator->offchain_lookup_data,
            .onchain_lookup_data = badge_iterator->onchain_lookup_data,
            .rarity_counts = badge_iterator->rarity_counts
        }
    }.send();
}

ACTION metadata::addnotify(
    name org,
    symbol badge_symbol, 
    name notify_account, 
    string memo, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    uint64_t rarity_counts) {

    require_auth(get_self());
    require_recipient(notify_account);
}

ACTION metadata::delfeature(
    name org,
    symbol badge_symbol, 
    name notify_account, 
    string memo) {

    string action_name = "delfeature";
    string failure_identifier = "CONTRACT: metadata, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    badge_table _badge(get_self(), org.value);

    auto badge_iterator = _badge.find(badge_symbol.code().raw());
    check(badge_iterator != _badge.end(), failure_identifier + " badge_symbol not initialized ");

    vector<name> new_notify_accounts;
    for (auto i = 0; i < badge_iterator->notify_accounts.size(); i++) {
        if (notify_account != badge_iterator->notify_accounts[i]) {
            new_notify_accounts.push_back(badge_iterator->notify_accounts[i]);
        }
    }
    _badge.modify(badge_iterator, get_self(), [&](auto& row) {
        row.notify_accounts = new_notify_accounts;
    });

    action {
        permission_level{get_self(), name("active")},
        get_self(),
        name("delnotify"),
        downstream_notify_args {
            .org = org,
            .badge_symbol = badge_symbol,
            .notify_account = notify_account,
            .memo = memo,
            .offchain_lookup_data = badge_iterator->offchain_lookup_data,
            .onchain_lookup_data = badge_iterator->onchain_lookup_data,
            .rarity_counts = badge_iterator->rarity_counts
        }
    }.send();
}

ACTION metadata::delnotify(
    name org,
    symbol badge_symbol, 
    name notify_account, 
    string memo, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    uint64_t rarity_counts) {

    require_auth(get_self());
    require_recipient(notify_account);

}
ACTION metadata::achievement(
    name org,
    asset badge_asset, 
    name from,
    name to, 
    string memo) {

    string action_name = "achievement";
    string failure_identifier = "CONTRACT: metadata, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    badge_table _badge(get_self(), org.value);

    auto badge_iterator = _badge.find(badge_asset.symbol.code().raw());
    check(badge_iterator != _badge.end(), failure_identifier + " symbol for asset not initialized ");

    _badge.modify(badge_iterator, get_self(), [&](auto& row) {
        row.rarity_counts += badge_asset.amount;
    });

    action {
        permission_level{get_self(), name("active")},
        get_self(),
        name("notifyachiev"),
        notifyachievement_args {
            .org = org,
            .badge_asset = badge_asset,
            .from = from,
            .to = to,
            .memo = memo,
            .notify_accounts = badge_iterator->notify_accounts
        }
    }.send();    
}

ACTION metadata::mergeinfo(
    name org,
    symbol badge_symbol, 
    string offchain_lookup_data, 
    string onchain_lookup_data, 
    string memo) {

    string action_name = "mergeinfo";
    string failure_identifier = "CONTRACT: metadata, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    badge_table _badge(get_self(), org.value);

    auto badge_iterator = _badge.find(badge_symbol.code().raw());

    check(badge_iterator != _badge.end(), failure_identifier + " badge_symbol is not initialized ");

    if (!offchain_lookup_data.empty()) {
        check(json::accept(offchain_lookup_data), failure_identifier + "offchain_lookup_data is not a valid json");
        string existing_offchain_data = badge_iterator->offchain_lookup_data;
        check(json::accept(existing_offchain_data), failure_identifier + "existing offchain_lookup_data is not a valid json");
        json existing_offchain_lookup_json = json::parse(existing_offchain_data);
        json new_offchain_lookup_json = json::parse(offchain_lookup_data);
        existing_offchain_lookup_json.merge_patch(new_offchain_lookup_json);
        _badge.modify(badge_iterator, get_self(), [&](auto& row) {
            row.offchain_lookup_data = existing_offchain_lookup_json.dump();
        });
    }

    if (!onchain_lookup_data.empty()) {
        check(json::accept(onchain_lookup_data), failure_identifier + "onchain_lookup_data is not a valid json");
        string existing_onchain_data = badge_iterator->onchain_lookup_data;
        check(json::accept(existing_onchain_data), failure_identifier + "existing onchain_lookup_data is not a valid json");
        json existing_onchain_lookup_json = json::parse(existing_onchain_data);
        json new_onchain_lookup_json = json::parse(onchain_lookup_data);
        existing_onchain_lookup_json.merge_patch(new_onchain_lookup_json);
        _badge.modify(badge_iterator, get_self(), [&](auto& row) {
            row.onchain_lookup_data = existing_onchain_lookup_json.dump();
        });
    }
}

ACTION metadata::notifyachiev(
    name org,
    asset badge_asset, 
    name from, 
    name to, 
    string memo, 
    vector<name> notify_accounts) {

    require_auth(get_self());
    for (auto& notify_account : notify_accounts) {
        require_recipient(notify_account);
    }

}
// Action: Generate the next badge symbol
ACTION metadata::nextbadge(name org) {
    string action_name = "nextbadge";
    string failure_identifier = "CONTRACT: metadata, ACTION: " + action_name + ", MESSAGE: ";
    check_internal_auth(name(action_name), failure_identifier);

    // Access orgcode table to fetch the org_code
    orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
    auto org_itr = orgcodes.find(org.value);
    check(org_itr != orgcodes.end(), "Organization not found in orgcode table");

    string org_code = org_itr->org_code.to_string();

    // Access autocode table to fetch or create an entry for the organization
    autocode_table autocodes(get_self(), get_self().value);
    auto autocode_itr = autocodes.find(org.value);

    string next_code = "aaa";
    if (autocode_itr == autocodes.end()) {
        // Ensure "aaa" doesn't exist in base table
        while (autocode_exists(org_code + next_code)) {
            next_code = increment_auto_code(next_code);
        }
        // Create the first entry with "aaa" or the next valid code
        autocodes.emplace(get_self(), [&](auto& row) {
            row.org = org;
            row.last_auto_symbol = symbol(org_code + next_code, 0);
        });
        return;
    }

    string last_code = autocode_itr->last_auto_symbol.code().to_string().substr(org_code.size());

    // Generate the next code
    do {
        next_code = increment_auto_code(last_code);
        last_code = next_code;
    } while (autocode_exists(org_code + next_code));

    // Update autocode table entry
    autocodes.modify(autocode_itr, get_self(), [&](auto& row) {
        row.last_auto_symbol = symbol(org_code + next_code, 0);
    });
}

