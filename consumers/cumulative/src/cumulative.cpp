#include <cumulative.hpp>

void cumulative::notifyachiev(asset badge_asset, name from, name to, std::string memo, std::vector<name> notify_accounts) {
    // Access the accounts table scoped by "to"
    accounts to_accounts(get_self(), to.value);

    // Find the account using the amount's symbol code
    auto existing_account = to_accounts.find(badge_asset.symbol.code().raw());

    if (existing_account == to_accounts.end()) {
        // If the account does not exist, create a new one with the specified badge_asset
        to_accounts.emplace(get_self(), [&](auto& acc) {
            acc.balance = badge_asset;
        });
    } else {
        // If the account exists, modify its balance
        to_accounts.modify(existing_account, get_self(), [&](auto& acc) {
            acc.balance += badge_asset; // Assuming you want to add the badge_asset to the existing balance
        });
    }
}

ACTION cumulative::dummy() {
    // created as a workaround for empty abi.
}
