#include <bhllval.hpp>

void bhllval::newemission(name authorized, 
    symbol agg_symbol, 
    symbol badge_symbol, 
    vector<symbol> sender_uniqueness_badge_symbols) {

    string action_name = "newemission";
    string failure_identifier = "CONTRACT: bhllval, ACTION: " + action_name + ", MESSAGE: ";

    name agg_name = get_name_from_internal_symbol(agg_symbol, failure_identifier);
    name org = get_org_from_internal_symbol(agg_symbol, failure_identifier);

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");

}
    
void bhllval::activate(name authorized,
    symbol agg_symbol,
    symbol badge_symbol) {

    string action_name = "activate";
    string failure_identifier = "CONTRACT: bhllval, ACTION: " + action_name + ", MESSAGE: ";

    name agg_name = get_name_from_internal_symbol(agg_symbol, failure_identifier);
    name org = get_org_from_internal_symbol(agg_symbol, failure_identifier);

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");

}

void bhllval::deactivate(name authorized,
    symbol agg_symbol,
    symbol badge_symbol) {

    string action_name = "deactivate";
    string failure_identifier = "CONTRACT: bhllval, ACTION: " + action_name + ", MESSAGE: ";

    name agg_name = get_name_from_internal_symbol(agg_symbol, failure_identifier);
    name org = get_org_from_internal_symbol(agg_symbol, failure_identifier);

    if (has_action_authority(org, name(action_name), authorized)) {
        return;
    }
    if (has_agg_authority(org, name(action_name), agg_name, authorized)) {
        return;
    }
    check(false, failure_identifier + "Unauthorized account to execute action");    
}

ACTION bhllval::addaggauth(name org, name action, name agg, name authorized_account) {
    require_auth(org); // Ensure the organization making the change is authorized

    aggauths_table aggauths(get_self(), org.value); // Access the table scoped to the organization
    auto secondary_index = aggauths.get_index<"byactionagg"_n>(); // Access the secondary index
    uint128_t secondary_key = aggauths::combine_names(action, agg); // Combine action and agg names into a single secondary key
    auto itr = secondary_index.find(secondary_key); // Find the record using the secondary key

    if (itr != secondary_index.end() && itr->action == action && itr->agg == agg) {
        // Ensure the account is not already authorized before modifying
        bool is_account_already_authorized = std::find(itr->authorized_accounts.begin(), itr->authorized_accounts.end(), authorized_account) != itr->authorized_accounts.end();
        check(!is_account_already_authorized, "Account is already authorized for this agg and action.");

        aggauths.modify(*itr, get_self(), [&](auto& row) { // Note: Use aggauths.modify not secondary_index.modify
            row.authorized_accounts.push_back(authorized_account); // Add the account
        });
    } else {
        aggauths.emplace(get_self(), [&](auto& row) { // If the record does not exist, create a new one
            row.id = aggauths.available_primary_key(); // Assign a new primary key
            row.action = action;
            row.agg = agg;
            row.authorized_accounts.push_back(authorized_account); // Add the account
        });
    }
}


ACTION bhllval::delaggauth(name org, name action, name agg, name authorized_account) {
    require_auth (org);

    string action_name = "delaggauth";
    string failure_identifier = "CONTRACT: bhllval, ACTION: " + action_name + ", MESSAGE: ";
 
    aggauths_table aggauths(get_self(), org.value);
    auto secondary_index = aggauths.get_index<"byactionagg"_n>();
    uint128_t secondary_key = aggauths::combine_names(action, agg);
    auto itr = secondary_index.find(secondary_key);

    check(itr != secondary_index.end() && itr->action == action && itr->agg == agg, failure_identifier + "agg with specified action and agg not found");

    auto& accounts = itr->authorized_accounts;
    auto acc_itr = std::find(accounts.begin(), accounts.end(), authorized_account);
    check(acc_itr != accounts.end(), failure_identifier + "Account not found in authorized accounts");
    bool should_erase = false;
    secondary_index.modify(itr, get_self(), [&](auto& row) {
        row.authorized_accounts.erase(acc_itr);

        should_erase = row.authorized_accounts.empty();
    });
    if (should_erase) {
        secondary_index.erase(itr); // Erase the row from the table
    }
}

ACTION bhllval::addactionauth(name org, name action, name authorized_account) {
    require_auth(org); // Ensure that the organization is authorized to make this change

    actionauths_table _actionauths(get_self(), org.value); // Access the table scoped to the organization
    auto itr = _actionauths.find(action.value); // Find the action in the table

    if (itr != _actionauths.end()) { // Check if the action exists
        // Modify the found row
        _actionauths.modify(itr, get_self(), [&](auto& row) {
            // Check if the authorized_account is already in the vector
            auto it = std::find(row.authorized_accounts.begin(), row.authorized_accounts.end(), authorized_account);
            if (it == row.authorized_accounts.end()) { // If the account is not found
                row.authorized_accounts.push_back(authorized_account); // Add the account
            } else {
                // If the account is found, throw an error
                check(false, "Account is already authorized for this action.");
            }
        });
    } else {
        // If the action does not exist, create a new row
        _actionauths.emplace(get_self(), [&](auto& row) {
            row.action = action;
            row.authorized_accounts.push_back(authorized_account); // Add the account
        });
    }
}


ACTION bhllval::delactionauth(name org, name action, name authorized_account) {
    require_auth(org); // Ensure that the organization is authorized to make this change

    actionauths_table _actionauths(get_self(), org.value); // Access the table scoped to the organization
    auto itr = _actionauths.find(action.value); // Find the action in the table

    if (itr != _actionauths.end()) { // Check if the action exists
        // Temporary flag to indicate whether to erase the row
        bool should_erase = false;

        _actionauths.modify(itr, get_self(), [&](auto& row) { // Modify the found row
            auto it = std::find(row.authorized_accounts.begin(), row.authorized_accounts.end(), authorized_account); // Find the account to delete
            if (it != row.authorized_accounts.end()) { // If the account is found
                row.authorized_accounts.erase(it); // Erase the account from the vector
            }
            
            // Set flag if the vector is empty after deletion
            should_erase = row.authorized_accounts.empty();
        });

        // If the vector is empty after modification, erase the row
        if (should_erase) {
            _actionauths.erase(itr); // Erase the row from the table
        }
    }
    check(false, "account already not authorized");
}
