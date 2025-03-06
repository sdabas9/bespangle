#include <msigs.hpp>

    ACTION consumesimple(
        name originating_contract,
        name originating_contract_key,
        name requester, 
        vector<name> approvers, 
        name to, 
        symbol badge_symbol, 
        string memo,
        string request_memo,
        time_point_sec expiration_time) {
        
        require_auth(requester);
        check(current_time_point().sec_since_epoch() < expiration_time.sec_since_epoch(), "Request expiration time must be in the future");

        // Extract org from badge_symbol
        name org = get_org_from_internal_symbol(badge_symbol, "consumesimple: ");

        // Scope by org
        sequence_index seq_table(get_self(), org.value);
        auto seq_itr = seq_table.find(org.value);
        uint64_t request_id;

        if (seq_itr == seq_table.end()) {
            seq_table.emplace(get_self(), [&](auto& row) {
                row.next_request_id = 1;
            });
            request_id = 1;
        } else {
            seq_table.modify(seq_itr, same_payer, [&](auto& row) {
                request_id = row.next_request_id++;
            });
        }

        // Initialize approver status
        vector<pair<name, name>> approver_statuses;
        for (const auto& approver : approvers) {
            approver_statuses.push_back({approver, "pending"_n});
        }

        // Store in request table
        request_index req_table(get_self(), org.value);
        req_table.emplace(get_self(), [&](auto& row) {
            row.request_id = request_id;
            row.action = "issuesimple"_n;
            row.requester = requester;
            row.originating_contract = originating_contract;
            row.originating_contract_key = originating_contract_key;
            row.approvers = approver_statuses;
            row.status = "pending"_n;
            row.expiration_time = expiration_time;
        });

        // Store in simissue table
        simissue_index sim_table(get_self(), org.value);
        sim_table.emplace(get_self(), [&](auto& row) {
            row.request_id = request_id;
            row.to = to;
            row.badge_symbol = badge_symbol;
            row.memo = memo;
        });
    }
    
    ACTION processone(name org, uint64_t request_id) {
        
        request_index req_table(get_self(), org.value);
        auto req_itr = req_table.find(request_id);
        check(req_itr != req_table.end(), "Request ID not found");

        if (current_time_point().sec_since_epoch() >= req_itr->expiration_time.sec_since_epoch()) {
            auto sim_itr = sim_table.find(request_id);
            check(sim_itr != sim_table.end(), "Matching simissue entry not found");                
            
            if (req_itr->status == "approved"_n) {
                action(
                    permission_level{get_self(), "active"_n},
                    "simmanager"_n,
                    "simpleissue"_n,
                    make_tuple(sim_itr->to, sim_itr->badge_symbol, sim_itr->memo)
                ).send();

                action(
                    permission_level{get_self(), "active"_n},
                    get_self(),
                    "notifications"_n,
                    make_tuple(requester, request_id, "processed"_n)
                ).send();
            } 
            req_table.erase(req_itr);
            sim_table.erase(sim_itr);
        } else {
            check(false, "not time yet to process this request_id");
        }
        
    }

    ACTION process(name org, uint16_t batch_size) {
        
        request_index req_table(get_self(), org.value);
        auto req_itr = req_table.begin();
        uint16_t processed = 0;
        
        simissue_index sim_table(get_self(), org.value);
        
        while (req_itr != req_table.end() && processed < batch_size) {
            uint64_t request_id = req_itr->request_id;
            if (current_time_point().sec_since_epoch() >= req_itr->expiration_time.sec_since_epoch()) {
                auto sim_itr = sim_table.find(request_id);
                check(sim_itr != sim_table.end(), "Matching simissue entry not found");                
                
                if (req_itr->status == "approved"_n) {
                    action(
                        permission_level{get_self(), "active"_n},
                        "simmanager"_n,
                        "simpleissue"_n,
                        make_tuple(sim_itr->to, sim_itr->badge_symbol, sim_itr->memo)
                    ).send();

                    action(
                        permission_level{get_self(), "active"_n},
                        get_self(),
                        "notifications"_n,
                        make_tuple(req_itr->originating_contract, req_itr>originating_contract_key, "processed"_n)
                    ).send();
                } 

                sim_table.erase(sim_itr);
                req_itr = req_table.erase(req_itr);
                processed++;

            } else {
                ++req_itr;
            }
        }
    }

    ACTION evidence(name requester, name org, uint64_t request_id, string stream_reason) {
        require_auth(requester);
        
        request_index req_table(get_self(), org.value);
        auto req_itr = req_table.find(request_id);
        check(req_itr != req_table.end(), "Request ID not found");
        check(current_time_point().sec_since_epoch() < req_itr->expiration_time.sec_since_epoch(), "Cannot modify expired request");
        
        // Reset approver statuses to pending
        vector<pair<name, name>> reset_approvers = req_itr->approvers;
        for (auto& entry : reset_approvers) {
            entry.second = "pending"_n;
        }
        
        // Modify request status and reset approvers
        req_table.modify(req_itr, same_payer, [&](auto& row) {
            row.approvers = reset_approvers;
            row.status = "pending"_n;
        });
        
        // Notify via notifications action
        action(
            permission_level{get_self(), "active"_n},
            get_self(),
            "notifications"_n,
            make_tuple(req_itr->originating_contract, req_itr->originating_contract_key, "pending"_n)
        ).send();
    }

    ACTION withdraw(name requester, name org, uint64_t request_id, string stream_reason) {
        require_auth(requester);
        
        request_index req_table(get_self(), org.value);
        auto req_itr = req_table.find(request_id);
        check(req_itr != req_table.end(), "Request ID not found");
        check(current_time_point().sec_since_epoch() < req_itr->expiration_time.sec_since_epoch(), "Cannot withdraw expired request");
        
        simissue_index sim_table(get_self(), org.value);
        auto sim_itr = sim_table.find(request_id);
        
        // Erase from request table
        req_table.erase(req_itr);
        
        // Erase from simissue table if exists
        if (sim_itr != sim_table.end()) {
            sim_table.erase(sim_itr);
        }
        
        // Notify via notifications action
        action(
            permission_level{get_self(), "active"_n},
            get_self(),
            "notifications"_n,
            make_tuple(requester, request_id, "withdrawn"_n)
        ).send();
    }

    ACTION notifications(name originating_contract, name originating_contract_key, name status) {
        require_recipient(originating_contract);
    }

    ACTION approve(name approver, name org, uint64_t request_id, string stream_reason) {
        require_auth(approver);

        request_index req_table(get_self(), org.value);
        auto req_itr = req_table.find(request_id);
        check(req_itr != req_table.end(), "Request ID not found");
        check(current_time_point().sec_since_epoch() < req_itr->expiration_time.sec_since_epoch(), "Cannot approve expired request");

        // Update approver status
        bool updated = false;
        vector<pair<name, name>> updated_approvers = req_itr->approvers;
        for (auto& entry : updated_approvers) {
            if (entry.first == approver && entry.second == "pending"_n) {
                entry.second = "approved"_n;
                updated = true;
                break;
            }
        }
        check(updated, "Approver not found or already approved");

        // Check if majority have approved
        int approved_count = 0;
        for (const auto& entry : updated_approvers) {
            if (entry.second == "approved"_n) {
                approved_count++;
            }
        }
        bool majority_approved = approved_count > (updated_approvers.size() / 2);

        // Modify request status if necessary
        req_table.modify(req_itr, same_payer, [&](auto& row) {
            row.approvers = updated_approvers;
            if (majority_approved) {
                row.status = "approved"_n;
            }
        });

        // Notify via notifications action if status changes
        if (majority_approved) {
            action(
                permission_level{get_self(), "active"_n},
                get_self(),
                "notifications"_n,
                make_tuple(req_itr->originating_contract, req_itr->originating_contract_key, "approved"_n)
            ).send();
        }
    }

    ACTION reject(name approver, name org, uint64_t request_id, string stream_reason) {
        require_auth(approver);

        request_index req_table(get_self(), org.value);
        auto req_itr = req_table.find(request_id);
        check(req_itr != req_table.end(), "Request ID not found");
        check(current_time_point().sec_since_epoch() < req_itr->expiration_time.sec_since_epoch(), "Cannot reject expired request");

        // Update approver status
        bool updated = false;
        vector<pair<name, name>> updated_approvers = req_itr->approvers;
        for (auto& entry : updated_approvers) {
            if (entry.first == approver && entry.second == "pending"_n) {
                entry.second = "rejected"_n;
                updated = true;
                break;
            }
        }
        check(updated, "Approver not found or already rejected");

        // Check if majority have rejected
        int rejected_count = 0;
        for (const auto& entry : updated_approvers) {
            if (entry.second == "rejected"_n) {
                rejected_count++;
            }
        }
        bool majority_rejected = rejected_count > (updated_approvers.size() / 2);

        // Modify request status if necessary
        req_table.modify(req_itr, same_payer, [&](auto& row) {
            row.approvers = updated_approvers;
            if (majority_rejected) {
                row.status = "rejected"_n;
            }
        });

        // Notify via notifications action if status changes
        if (majority_rejected) {
            action(
                permission_level{get_self(), "active"_n},
                get_self(),
                "notifications"_n,
                make_tuple(req_itr->originating_contract, req_itr->originating_contract_key, "rejected"_n)
            ).send();
        }
    }





   