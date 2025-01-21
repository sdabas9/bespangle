#include <eosio/eosio.hpp>
#include <eosio/transaction.hpp>
#include <vector>

using namespace eosio;

class [[eosio::contract]] msig_proposer : public contract {
public:
    using contract::contract;

ACTION simissuemsig(
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
        action givesimple_action = action(
            permission_level{get_self(), "active"_n}, // Permission level
            "simmanager"_n,                           // Target contract
            "givesimple"_n,                           // Action name
            std::make_tuple(get_self(), badge_symbol, amount, to, memo) // Arguments
        );

        // Serialize the action into a transaction
        transaction trx;
        trx.actions.emplace_back(givesimple_action);
        trx.expiration = time_point_sec(current_time_point().sec_since_epoch() + expiry_duration); // Expiration time

        // Construct requested approvals
        std::vector<permission_level> requested_approvals;
        for (auto actor : actors) {
            requested_approvals.push_back(permission_level{actor, "active"_n});
        }

        // Call eosio.msig's propose action
        action(
            permission_level{proposer, "active"_n},  // Proposer's permission
            "eosio.msig"_n,                          // eosio.msig system contract
            "propose"_n,                             // Action name
            std::make_tuple(proposer, proposal_name, requested_approvals, trx)
        ).send();
    }
};
