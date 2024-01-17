#include <authority.hpp>

ACTION authority::addauth(name contract, name action, name authorized_contract) {
        require_auth(get_self());

        auth_table _auth(get_self(), get_self().value);

        // Find or create the authority entry
        auto secondary_key = (uint128_t)contract.value << 64 | action.value;
        auto secondary_index = _auth.get_index<"bycontract"_n>();
        auto itr = secondary_index.find(secondary_key);

        if (itr == secondary_index.end() || itr->contract!=contract || itr->action!=action) {
            // Create a new authority entry if it doesn't exist
            _auth.emplace(get_self(), [&](auto& new_auth) {
                new_auth.id = _auth.available_primary_key();
                new_auth.contract = contract;
                new_auth.action = action;
                new_auth.authorized_contracts.push_back(authorized_contract);
            });
        } else {
            // Add to the existing authority's authorized contracts
            secondary_index.modify(itr, get_self(), [&](auto& mod_auth) {
                check(std::find(mod_auth.authorized_contracts.begin(), mod_auth.authorized_contracts.end(), authorized_contract) == mod_auth.authorized_contracts.end(), "Contract is already authorized");
                mod_auth.authorized_contracts.push_back(authorized_contract);
            });
        }
    }

    // Remove a single authorized contract based on contract and action
ACTION authority::removeauth(name contract, name action, name authorized_contract) {
        require_auth(get_self());

        auth_table _auth(get_self(), get_self().value);

        // Find the authority entry
        auto secondary_key = (uint128_t)contract.value << 64 | action.value;
        auto secondary_index = _auth.get_index<"bycontract"_n>();
        auto itr = secondary_index.find(secondary_key);

        check(itr != secondary_index.end() && itr->contract==contract && itr->action==action, "Authority with this contract and action does not exist");

        bool erase_row = false;
        // Remove the specified authorized contract
        secondary_index.modify(itr, get_self(), [&](auto& mod_auth) {
            auto& contracts = mod_auth.authorized_contracts;
            auto contract_itr = std::find(contracts.begin(), contracts.end(), authorized_contract);
            check(contract_itr != contracts.end(), "Contract is not authorized");
            contracts.erase(contract_itr);
            if(contracts.size() == 0) {
               erase_row = true; 
            }
        });

        if(erase_row) {
            secondary_index.erase(itr);
        }
    }

ACTION authority::hasauth(name contract, name action, name account) {
        auth_table _auth(get_self(), get_self().value);

        // Find the authority entry
        auto secondary_key = (uint128_t)contract.value << 64 | action.value;
        auto secondary_index = _auth.get_index<"bycontract"_n>();
        auto itr = secondary_index.find(secondary_key);

        if (itr == secondary_index.end()) {
            check(false, "Authority contract : no authority found"); // No authority found for the specified contract and action
        }

        // Check if the account is in the authorized_contracts vector
        const auto& auth = *itr;
        check(std::find(auth.authorized_contracts.begin(), auth.authorized_contracts.end(), account) != auth.authorized_contracts.end(), "Account not is authorized list");
}