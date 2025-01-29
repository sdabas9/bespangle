#include <msigs.hpp>


ACTION msigs::customperm(name perm_name, std::vector<name> accounts) {
        // Ensure the account has authorized this action
        require_auth(get_self());

        // Check if the accounts vector is not empty
        check(!accounts.empty(), "Accounts list cannot be empty.");

        // Sort accounts
        std::sort(accounts.begin(), accounts.end());

        // Build the authority object with default weight of 1 for each account
        std::vector<permission_level_weight> auth_accounts;
        for (const auto& acc : accounts) {
            auth_accounts.push_back({
                .permission = permission_level{acc, "active"_n}, // Default to "active" permission
                .weight = 1                                    // Default weight of 1
            });
        }

        // Calculate the threshold as 50% or more of the total accounts
        uint32_t total_weight = auth_accounts.size();
        uint32_t threshold = (total_weight % 2 == 0) ? total_weight / 2 : (total_weight / 2) + 1;

        // Construct the authority object
        authority auth_data = {
            .threshold = threshold,
            .keys = {}, // No keys
            .accounts = auth_accounts,
            .waits = {} // No time delays for this example
        };

        // Construct the auth struct
        auth new_auth = {
            .account = get_self(),
            .permission_name = perm_name,
            .parent = "owner"_n,
            .auth_data = auth_data
        };

        // Use the `updateauth` action to create or update the custom permission
        action(
            permission_level{get_self(), "owner"_n}, // Only the "owner" permission can set permissions
            "eosio"_n,
            "updateauth"_n,
            new_auth // Pass the struct as the argument
        ).send();
    }
   




ACTION msigs::simissuemsig(
                     name proposal_name, 
                     std::vector<name> actors, 
                     symbol badge_symbol, 
                     uint64_t amount, 
                     name to, 
                     uint32_t expiry_duration, 
                     std::string memo) {
        require_auth(get_self()); // Ensure proposer is authorized

        // Validate input
        check(actors.size() > 0, "Actors list must not be empty");
        check(amount > 0, "Amount must be greater than zero");
        check(is_account(to), "Recipient account does not exist");
        check(memo.size() <= 256, "Memo has more than 256 characters");

        // Construct the action for `simmanager::givesimple`
    //    action givesimple_action = 
        action(
            permission_level{get_self(), "testperm"_n}, // Permission level
            "simmanageryy"_n,                           // Target contract
            "givesimple"_n,                           // Action name
            std::make_tuple(get_self(), badge_symbol, amount, to, memo) // Arguments
        ).send();

        // Serialize the action into a transaction
    /*    transaction trx;
        trx.actions.emplace_back(givesimple_action);
        trx.expiration = time_point_sec(current_time_point().sec_since_epoch() + expiry_duration); // Expiration time
        std::sort(actors.begin(), actors.end());
        // Construct requested approvals
        std::vector<permission_level> requested_approvals;
        for (auto actor : actors) {
            requested_approvals.push_back(permission_level{actor, "active"_n});
        }
        
        // Call eosio.msig's propose action
        action(
            permission_level{get_self(), "active"_n},  // Proposer's permission
            "eosio.msig"_n,                          // eosio.msig system contract
            "propose"_n,                             // Action name
            std::make_tuple(get_self(), proposal_name, requested_approvals, trx)
        ).send(); */
    }
