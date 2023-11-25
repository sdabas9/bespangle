#!/bin/sh

ORCHESTRATOR_CONTRACT="orchestrator"
BILLING_CONTRACT="billingxxxxx"

SIMPLEBADGE_CONTRACT="simplebadgex"
GOTCHABADGE_CONTRACT="gotchabadgex"
SERIESBADGE_CONTRACT="seriesbadgex"
ANTIBADGE_CONTRACT="antibadgexxx"

AABADGE_CONTRACT="aabadgexxxxx"
ATOMIC_ASSETS_CONTRACT="atomicassets"
AANFT_CONTRACT="aanftxxxxxxx"

ORG_INTERFACE_CONTRACT="interface111"
SERIES_VALIDATION_CONTRACT="seriesval111"
ROUND_MANAGER_CONTRACT="roundmanager"
MUTUAL_RECOGNITION_MANAGER_CONTRACT="mrmanagerxxx"

BOUNDED_AGG_CONTRACT="boundedaggxx"
BOUNDED_STATS_CONTRACT="boundedstats"
BOUNDED_HLL_CONTRACT="boundedhllxx"

ROUND_DEFERRED_CONTRACT="rounddelayed"

EOSIO_TOKEN_POOL_CONTRACT="eosiotknpool"
REDEEM_CONTRACT="redeemsbtxxx"

CUMULATIVE_CONTRACT="cumulativexx"
STATISTICS_CONTRACT="statisticsxx"

HLL_EMITTER_CONTRACT="hllemitterxx"


GIVER_REP_CONTRACT="giverrepxxxx"
ANDEMITTER_CONTRACT="andemitterxx"
NOTIFICATION_CONTRACT="notification"
MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT="mrnotifyxxxx"
TAP_CONTRACT="tapxxxxxxxxx"
CHECKS_CONTRACT="checksxxxxxx"

CLAIMASSET_CONTRACT="dummy"



CLEOS_URL="http://jungle4.cryptolions.io"


# Define paths
METADATA_PATH="core/metadata"
ORGBILL_PATH="core/orgbill"
SIMPLEBADGE_PATH="producers/simplebadge"
GOTCHABADGE_PATH="producers/gotchabadge"
SERIESBADGE_PATH="producers/seriesbadge"
ANTIBADGE_PATH="producers/antibadge"
AABADGE_PATH="consumers/aabadge"
# AANFT_PATH="consumers/aanft" # Commented out as the original code is commented
ORG_PATH="org"
SERVAL_PATH="validations/serval"
ROUNDMANAGER_PATH="core/roundmanager"
MRMANAGER_PATH="interface/mrmanager"
BOUNDAGG_PATH="consumers/boundagg"
BOUNDEDSTATS_PATH="consumers/boundedstats"
BOUNDEDHLL_PATH="consumers/boundedhll"
DEFERRED_PATH="core/deferred"
EOSIOTKNPOOL_PATH="core/eosiotknpool"
REDEEM_PATH="core/redeem"
CUMULATIVE_PATH="consumers/cumulative"
STATISTICS_PATH="consumers/statistics"
HLLEMITTER_PATH="consumers/hllemitter"
GIVERREP_PATH="reputation/giverrep"
ANDEMITTER_PATH="consumers/andemitter"
NOTIFICATION_PATH="core/notification"
MRNOTIFY_PATH="core/mrnotify"
TAP_PATH="consumers/tap"
CHECKS_PATH="core/checks"


parse_arguments() {
    ACTION=$1
    CONTRACT=$2
}

# Function to check if a contract should be processed
should_process() {
    local contract_name=$1
    if [ "$CONTRACT" = "all" ] || [ "$CONTRACT" = "$contract_name" ]; then
        return 0  # true in shell script
    else
        return 1  # false in shell script
    fi
}

# Modified build_contract function
build_contract() {
    local contract_name=$1
    local contract_path=$2
    local cmake_params=$3

    if should_process $contract_name && ([ "$ACTION" = "build" ] || [ "$ACTION" = "both" ]); then
        pushd $contract_path > /dev/null
        cmake . $cmake_params
        eosio-cpp -abigen -I ./include -R ./resource -contract $contract_name -o ${contract_name}.wasm src/${contract_name}.cpp
        popd > /dev/null
    fi
}

# Modified deploy_contract function
deploy_contract() {
    local contract_name=$1
    local contract_path=$2
    local contract_file=$3

    if should_process $contract_file && ([ "$ACTION" = "deploy" ] || [ "$ACTION" = "both" ]); then
        cleos -u $CLEOS_URL set contract $contract_name $contract_path "${contract_file}.wasm" "${contract_file}.abi" -p $contract_name@active
        cleos -u $CLEOS_URL set account permission $contract_name active --add-code -p $contract_name@active
    fi
}

# Parse arguments
parse_arguments $1 $2

build_contract "metadata" $METADATA_PATH "-DBILLING_CONTRACT=$BILLING_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT"
build_contract "orgbill" $ORGBILL_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT"
build_contract "simplebadge" $SIMPLEBADGE_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT"
build_contract "gotchabadge" $GOTCHABADGE_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT"
build_contract "seriesbadge" $SERIESBADGE_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT"
build_contract "antibadge" $ANTIBADGE_PATH "-DCUMULATIVE_CONTRACT_NAME=$CUMULATIVE_CONTRACT -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DCHECKS_CONTRACT_NAME=$CHECKS_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT"
build_contract "aabadge" $AABADGE_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DATOMIC_ASSETS_CONTRACT=$ATOMIC_ASSETS_CONTRACT"

# ... [Previous build_contract calls] ...

# Org
build_contract "org" $ORG_PATH "-DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DGOTCHABADGE_CONTRACT=$GOTCHABADGE_CONTRACT -DCLAIMASSET_CONTRACT=$CLAIMASSET_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DAABADGE_CONTRACT=$AABADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DROUNDS_CONTRACT=$ROUNDS_CONTRACT -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DASYNC_CONTRACT=$ASYNC_CONTRACT"

# Serval
build_contract "serval" $SERVAL_PATH "-DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT"

# Roundmanager
build_contract "roundmanager" $ROUNDMANAGER_PATH "-DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT -DORG_CHECKS_CONTRACT_NAME=$ORG_INTERFACE_CONTRACT"

# Mrmanager
build_contract "mrmanager" $MRMANAGER_PATH "-DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DMUTUAL_RECOGNITION_NOTIFICATION_CONTRACT=$MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT -DORG_CHECKS_CONTRACT=$ORG_INTERFACE_CONTRACT"

# Boundagg
build_contract "boundagg" $BOUNDAGG_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT"

# Boundedstats
build_contract "boundedstats" $BOUNDEDSTATS_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT"

# Boundedhll
build_contract "boundedhll" $BOUNDEDHLL_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT"

# Deferred
build_contract "deferred" $DEFERRED_PATH "-DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT"

# Eosiotknpool
build_contract "eosiotknpool" $EOSIOTKNPOOL_PATH "-DREDEEM_CONTRACT_NAME=$REDEEM_CONTRACT"

# Redeem
build_contract "redeem" $REDEEM_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT"

# Cumulative
build_contract "cumulative" $CUMULATIVE_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT"

# Statistics
build_contract "statistics" $STATISTICS_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DCUMULATIVE_CONTRACT_NAME=$CUMULATIVE_CONTRACT"

# Hllemitter
build_contract "hllemitter" $HLLEMITTER_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT"

# Giverrep
build_contract "giverrep" $GIVERREP_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DSTATS_CONTRACT_NAME=$STATISTICS_CONTRACT -DCUMULATIVE_CONTRACT_NAME=$CUMULATIVE_CONTRACT  -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT"

# Andemitter
build_contract "andemitter" $ANDEMITTER_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DSIMPLEBADGE_CONTRACT_NAME=$SIMPLEBADGE_CONTRACT -DCLAIMASSET_CONTRACT_NAME=$CLAIMASSET_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT"

# Notification
build_contract "notification" $NOTIFICATION_PATH "-DORG_INTERFACE_CONTRACT_NAME=$ORG_INTERFACE_CONTRACT -DSIMPLEBADGE_CONTRACT_NAME=$SIMPLEBADGE_CONTRACT -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DANDEMITTER_CONTRACT_NAME=$ANDEMITTER_CONTRACT -DCLAIMASSET_CONTRACT_NAME=$CLAIMASSET_CONTRACT -DSERIESBADGE_CONTRACT_NAME=$SERIESBADGE_CONTRACT -DROUND_MANAGER_CONTRACT_NAME=$ROUND_MANAGER_CONTRACT -DREDEEM_CONTRACT_NAME=$REDEEM_CONTRACT -DANTIBADGE_CONTRACT_NAME=$ANTIBADGE_CONTRACT -DDEFERRED_CONTRACT_NAME=$ROUND_DEFERRED_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT"

# Mrnotify
build_contract "mrnotify" $MRNOTIFY_PATH "-DMUTUAL_RECOGNITION_CONTRACT_NAME=$GOTCHABADGE_CONTRACT -DMUTUAL_RECOGNITION_INTERFACE_CONTRACT_NAME=$ORG_INTERFACE_CONTRACT"

# Tap
build_contract "tap" $TAP_PATH "-DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT"

# Checks
build_contract "checks" $CHECKS_PATH "-DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DBOUNDED_AGG_CONTRACT=$BOUNDED_AGG_CONTRACT"




# Metadata
deploy_contract "$ORCHESTRATOR_CONTRACT" "$METADATA_PATH" "metadata"

# Orgbill
deploy_contract "$BILLING_CONTRACT" "$ORGBILL_PATH" "orgbill"

# Simplebadge
deploy_contract "$SIMPLEBADGE_CONTRACT" "$SIMPLEBADGE_PATH" "simplebadge"

# Gotchabadge
deploy_contract "$GOTCHABADGE_CONTRACT" "$GOTCHABADGE_PATH" "gotchabadge"

# Seriesbadge
deploy_contract "$SERIESBADGE_CONTRACT" "$SERIESBADGE_PATH" "seriesbadge"

# Antibadge
deploy_contract "$ANTIBADGE_CONTRACT" "$ANTIBADGE_PATH" "antibadge"

# Aabadge
deploy_contract "$AABADGE_CONTRACT" "$AABADGE_PATH" "aabadge"

# Org
deploy_contract "$ORG_INTERFACE_CONTRACT" "$ORG_PATH" "org"

# Serval
deploy_contract "$SERIES_VALIDATION_CONTRACT" "$SERVAL_PATH" "serval"

# Roundmanager
deploy_contract "$ROUND_MANAGER_CONTRACT" "$ROUNDMANAGER_PATH" "roundmanager"

# Mrmanager
deploy_contract "$MUTUAL_RECOGNITION_MANAGER_CONTRACT" "$MRMANAGER_PATH" "mrmanager"

# Boundagg
deploy_contract "$BOUNDED_AGG_CONTRACT" "$BOUNDAGG_PATH" "boundagg"

# Boundedstats
deploy_contract "$BOUNDED_STATS_CONTRACT" "$BOUNDEDSTATS_PATH" "boundedstats"

# Boundedhll
deploy_contract "$BOUNDED_HLL_CONTRACT" "$BOUNDEDHLL_PATH" "boundedhll"

# Deferred
deploy_contract "$ROUND_DEFERRED_CONTRACT" "$DEFERRED_PATH" "deferred"

# Eosiotknpool
deploy_contract "$EOSIO_TOKEN_POOL_CONTRACT" "$EOSIOTKNPOOL_PATH" "eosiotknpool"

# Redeem
deploy_contract "$REDEEM_CONTRACT" "$REDEEM_PATH" "redeem"

# Cumulative
deploy_contract "$CUMULATIVE_CONTRACT" "$CUMULATIVE_PATH" "cumulative"

# Statistics
deploy_contract "$STATISTICS_CONTRACT" "$STATISTICS_PATH" "statistics"

# Hllemitter
deploy_contract "$HLL_EMITTER_CONTRACT" "$HLLEMITTER_PATH" "hllemitter"

# Giverrep
deploy_contract "$GIVER_REP_CONTRACT" "$GIVERREP_PATH" "giverrep"

# Andemitter
deploy_contract "$ANDEMITTER_CONTRACT" "$ANDEMITTER_PATH" "andemitter"

# Notification
deploy_contract "$NOTIFICATION_CONTRACT" "$NOTIFICATION_PATH" "notification"

# Mrnotify
deploy_contract "$MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT" "$MRNOTIFY_PATH" "mrnotify"

# Tap
deploy_contract "$TAP_CONTRACT" "$TAP_PATH" "tap"

# Checks
deploy_contract "$CHECKS_CONTRACT" "$CHECKS_PATH" "checks"

