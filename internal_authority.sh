#!/bin/bash

# Source the contracts file
source ./accounts.sh

# Base URL for the cleos command
CLEOS_URL="http://jungle4.cryptolions.io"

# Function to push transactions to the blockchain
push_transaction() {
    local account=$1
    local contract=$2
    local action=$3
    local authorized_contract=$4

    # Construct the JSON payload
    local payload=$(cat <<-END
    {
      "delay_sec": 0,
      "max_cpu_usage_ms": 0,
      "actions": [
        {
          "account": "$account",
          "name": "$action",
          "data": {
            "contract": "$contract",
            "action": "$action",
            "authorized_contract": "$authorized_contract"
          },
          "authorization": [
            {
              "actor": "$account",
              "permission": "active"
            }
          ]
        }
      ]
    }
END
)

    # Use cleos to push the transaction
    echo "Pushing transaction for action $action on contract $contract"
    cleos -u $CLEOS_URL push transaction "$payload"
}

# Use variables sourced from accounts.sh in the function calls
push_transaction "$AUTHORITY_CONTRACT" "$SIMPLEBADGE_CONTRACT" "create" "$SIMPLE_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "initbadge" "$SIMPLEBADGE_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "addfeature" "$SIMPLE_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$SIMPLEBADGE_CONTRACT" "issue" "$SIMPLE_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "achievement" "$SIMPLEBADGE_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ANDEMITTER_CONTRACT" "newemission" "$ANDEMITTER_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "addfeature" "$ANDEMITTER_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ANDEMITTER_CONTRACT" "activate" "$ANDEMITTER_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$SIMPLEBADGE_CONTRACT" "issue" "$ANDEMITTER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "addfeature" "$BOUNDED_AGG_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$BOUNDED_AGG_CONTRACT" "initagg" "$BOUNDED_AGG_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$BOUNDED_STATS_CONTRACT" "activate" "$BOUNDED_AGG_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$BOUNDED_AGG_CONTRACT" "initseq" "$BOUNDED_AGG_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$BOUNDED_AGG_CONTRACT" "actseq" "$BOUNDED_AGG_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$MUTUAL_RECOGNITION_CONTRACT" "create" "$MUTUAL_RECOGNITION_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "initbadge" "$MUTUAL_RECOGNITION_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "addfeature" "$MUTUAL_RECOGNITION_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$MUTUAL_RECOGNITION_CONTRACT" "issue" "$MUTUAL_RECOGNITION_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "achievement" "$MUTUAL_RECOGNITION_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "addfeature" "$HLL_EMITTER_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$HLL_EMITTER_CONTRACT" "newemission" "$HLL_EMITTER_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$HLL_EMITTER_CONTRACT" "activate" "$HLL_EMITTER_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$SIMPLEBADGE_CONTRACT" "issue" "$HLL_EMITTER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "addfeature" "$GIVER_REP_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$GIVER_REP_CONTRACT" "newemission" "$GIVER_REP_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$GIVER_REP_CONTRACT" "activate" "$GIVER_REP_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$SIMPLEBADGE_CONTRACT" "issue" "$GIVER_REP_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$BOUNDED_AGG_CONTRACT" "addinitbadge" "$BOUNDED_AGG_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$BOUNDED_AGG_CONTRACT" "endseq" "$BOUNDED_AGG_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$ORCHESTRATOR_CONTRACT" "addfeature" "$BOUNDED_HLL_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$BOUNDED_HLL_CONTRACT" "newemission" "$BOUNDED_HLL_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$BOUNDED_HLL_CONTRACT" "activate" "$BOUNDED_HLL_MANAGER_CONTRACT"
push_transaction "$AUTHORITY_CONTRACT" "$SIMPLEBADGE_CONTRACT" "issue" "$BOUNDED_HLL_CONTRACT"

# Ensure to validate and test thoroughly before running this in a production environment.

