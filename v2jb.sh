#!/bin/sh

BILLING_CONTRACT="billingxxxxx"

ROUND_MANAGER_CONTRACT="roundmanager"
MUTUAL_RECOGNITION_MANAGER_CONTRACT="mrmanagerxxx"
SIMPLE_MANAGER_CONTRACT="simmanagerzz"
SERIES_MANAGER_CONTRACT="sermanagerxx"
ANTIBADGE_MANAGER_CONTRACT="abmanagerxxx"
REPUTATION_MANAGER_CONTRACT="repmanagerxx"

ORG_CONTRACT="orgzzzzzzzzz"
SERIES_VALIDATION_CONTRACT="seriesval111"
SIMPLE_VALIDATION_CONTRACT="simplevalzzz"
MUTUAL_RECOGNITION_VALIDATION_CONTRACT="gotchavalxxx"
ANTIBADGE_VALIDATION_CONTRACT="antibadgeval"
REPUTATION_MANAGER_VALIDATION_CONTRACT="repvalxxxxxx"

CUMULATIVE_CONTRACT="cumulativezz"
STATISTICS_CONTRACT="statisticszz"
HLL_EMITTER_CONTRACT="hllemitterxx"
GIVER_REP_CONTRACT="giverrepxxxx"
ANDEMITTER_CONTRACT="andemitterxx"
BOUNDED_AGG_CONTRACT="boundedaggxx"
BOUNDED_STATS_CONTRACT="boundedstats"
BOUNDED_HLL_CONTRACT="boundedhllxx"
AABADGE_CONTRACT="aabadgexxxxx"
AANFT_CONTRACT="aanftxxxxxxx"

ORCHESTRATOR_CONTRACT="orchzzzzzzzz"





ATOMIC_ASSETS_CONTRACT="atomicassets"
ROUND_DEFERRED_CONTRACT="rounddelayed"
EOSIO_TOKEN_POOL_CONTRACT="eosiotknpool"
REDEEM_CONTRACT="redeemsbtxxx"

SIMPLEBADGE_CONTRACT="simplebadgez"
GOTCHABADGE_CONTRACT="gotchabadgex"
SERIESBADGE_CONTRACT="seriesbadgex"
ANTIBADGE_CONTRACT="antibadgexxx"

NOTIFICATION_CONTRACT="notification"
AUTHORITY_CONTRACT="authorityzzz"

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
SIMVAL_PATH="validations/simpleval"
MRVAL_PATH="validations/mrval"
ABVAL_PATH="validations/abval"
REPVAL_PATH="validations/repval"
ROUNDMANAGER_PATH="core/roundmanager"
MRMANAGER_PATH="interface/mrmanager"
SIMMANAGER_PATH="interface/simmanager"
SERMANAGER_PATH="interface/sermanager"
ABMANAGER_PATH="interface/abmanager"
REPMANAGER_PATH="interface/repmanager"
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
AUTHORITY_PATH="core/authority"
TAP_PATH="consumers/tap"
CHECKS_PATH="core/checks"


parse_arguments() {
    ACTION=$1
    CONTRACT=$2
}

# Split CONTRACT into an array based on comma separation
split_contract_names() {
    IFS=',' read -ra ADDR <<< "$CONTRACT"
    CONTRACT_NAMES=("${ADDR[@]}")
}

# Updated should_process function to support comma-separated contract names
should_process() {
    local contract_name=$1
    if [ "$CONTRACT" = "all" ]; then
        return 0  # true in shell script for "all"
    fi
    for item in "${CONTRACT_NAMES[@]}"; do
        if [ "$item" = "$contract_name" ]; then
            return 0  # true in shell script if found
        fi
    done
    return 1  # false in shell script if not found
}

# Your existing build_contract and deploy_contract functions here

# Parse arguments and prepare contract names
parse_arguments $1 $2
split_contract_names

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

_D_PART="-DBILLING_CONTRACT=$BILLING_CONTRACT -DROUND_MANAGER_CONTRACT=$ROUND_MANAGER_CONTRACT -DMUTUAL_RECOGNITION_MANAGER_CONTRACT=$MUTUAL_RECOGNITION_MANAGER_CONTRACT -DSIMPLE_MANAGER_CONTRACT=$SIMPLE_MANAGER_CONTRACT -DSERIES_MANAGER_CONTRACT=$SERIES_MANAGER_CONTRACT -DANTIBADGE_MANAGER_CONTRACT=$ANTIBADGE_MANAGER_CONTRACT -DREPUTATION_MANAGER_CONTRACT=$REPUTATION_MANAGER_CONTRACT -DORG_CONTRACT=$ORG_CONTRACT -DSERIES_VALIDATION_CONTRACT=$SERIES_VALIDATION_CONTRACT -DSIMPLE_VALIDATION_CONTRACT=$SIMPLE_VALIDATION_CONTRACT -DMUTUAL_RECOGNITION_VALIDATION_CONTRACT=$MUTUAL_RECOGNITION_VALIDATION_CONTRACT -DANTIBADGE_VALIDATION_CONTRACT=$ANTIBADGE_VALIDATION_CONTRACT -DREPUTATION_MANAGER_VALIDATION_CONTRACT=$REPUTATION_MANAGER_VALIDATION_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DSTATISTICS_CONTRACT=$STATISTICS_CONTRACT -DHLL_EMITTER_CONTRACT=$HLL_EMITTER_CONTRACT -DGIVER_REP_CONTRACT=$GIVER_REP_CONTRACT -DANDEMITTER_CONTRACT=$ANDEMITTER_CONTRACT -DBOUNDED_AGG_CONTRACT=$BOUNDED_AGG_CONTRACT -DBOUNDED_STATS_CONTRACT=$BOUNDED_STATS_CONTRACT -DBOUNDED_HLL_CONTRACT=$BOUNDED_HLL_CONTRACT -DAABADGE_CONTRACT=$AABADGE_CONTRACT -DAANFT_CONTRACT=$AANFT_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DATOMIC_ASSETS_CONTRACT=$ATOMIC_ASSETS_CONTRACT -DROUND_DEFERRED_CONTRACT=$ROUND_DEFERRED_CONTRACT -DEOSIO_TOKEN_POOL_CONTRACT=$EOSIO_TOKEN_POOL_CONTRACT -DREDEEM_CONTRACT=$REDEEM_CONTRACT -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DGOTCHABADGE_CONTRACT=$GOTCHABADGE_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DANTIBADGE_CONTRACT=$ANTIBADGE_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DAUTHORITY_CONTRACT=$AUTHORITY_CONTRACT -DTAP_CONTRACT=$TAP_CONTRACT -DCHECKS_CONTRACT=$CHECKS_CONTRACT -DCLAIMASSET_CONTRACT=$CLAIMASSET_CONTRACT"


build_contract "metadata" $METADATA_PATH "$_D_PART"
build_contract "orgbill" $ORGBILL_PATH "$_D_PART"
build_contract "simplebadge" $SIMPLEBADGE_PATH "$_D_PART"
build_contract "gotchabadge" $GOTCHABADGE_PATH "$_D_PART"
build_contract "seriesbadge" $SERIESBADGE_PATH "$_D_PART"
build_contract "antibadge" $ANTIBADGE_PATH "$_D_PART"
build_contract "aabadge" $AABADGE_PATH "$_D_PART"
build_contract "org" $ORG_PATH "$_D_PART"
build_contract "serval" $SERVAL_PATH "$_D_PART"
build_contract "simpleval" $SIMVAL_PATH "$_D_PART"
build_contract "mrval" $MRVAL_PATH "$_D_PART"
build_contract "abval" $ABVAL_PATH "$_D_PART"
build_contract "repval" $REPVAL_PATH "$_D_PART"
build_contract "roundmanager" $ROUNDMANAGER_PATH "$_D_PART"
build_contract "mrmanager" $MRMANAGER_PATH "$_D_PART"
build_contract "simmanager" $SIMMANAGER_PATH "$_D_PART"
build_contract "sermanager" $SERMANAGER_PATH "$_D_PART"
build_contract "repmanager" $REPMANAGER_PATH "$_D_PART"
build_contract "abmanager" $ABMANAGER_PATH "$_D_PART"
build_contract "boundagg" $BOUNDAGG_PATH "$_D_PART"
build_contract "boundedstats" $BOUNDEDSTATS_PATH "$_D_PART"
build_contract "boundedhll" $BOUNDEDHLL_PATH "$_D_PART"
build_contract "deferred" $DEFERRED_PATH "$_D_PART"
build_contract "eosiotknpool" $EOSIOTKNPOOL_PATH "$_D_PART"
build_contract "redeem" $REDEEM_PATH "$_D_PART"
build_contract "cumulative" $CUMULATIVE_PATH "$_D_PART"
build_contract "statistics" $STATISTICS_PATH "$_D_PART"
build_contract "hllemitter" $HLLEMITTER_PATH "$_D_PART"
build_contract "giverrep" $GIVERREP_PATH "$_D_PART"
build_contract "andemitter" $ANDEMITTER_PATH "$_D_PART"
build_contract "notification" $NOTIFICATION_PATH "$_D_PART"
build_contract "authority" $AUTHORITY_PATH "$_D_PART"
build_contract "tap" $TAP_PATH "$_D_PART"
build_contract "checks" $CHECKS_PATH "$_D_PART"




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
deploy_contract "$ORG_CONTRACT" "$ORG_PATH" "org"

# Serval
deploy_contract "$SERIES_VALIDATION_CONTRACT" "$SERVAL_PATH" "serval"

# Simpleval
deploy_contract "$SIMPLE_VALIDATION_CONTRACT" "$SIMVAL_PATH" "simpleval"

# Mrval
deploy_contract "$MUTUAL_RECOGNITION_VALIDATION_CONTRACT" "$MRVAL_PATH" "mrval"

# Abval
deploy_contract "$ANTIBADGE_VALIDATION_CONTRACT" "$ABVAL_PATH" "abval"

# Repval
deploy_contract "$REPUTATION_MANAGER_VALIDATION_CONTRACT" "$REPVAL_PATH" "repval"

# Roundmanager
deploy_contract "$ROUND_MANAGER_CONTRACT" "$ROUNDMANAGER_PATH" "roundmanager"

# Mrmanager
deploy_contract "$MUTUAL_RECOGNITION_MANAGER_CONTRACT" "$MRMANAGER_PATH" "mrmanager"

# Simmanager
deploy_contract "$SIMPLE_MANAGER_CONTRACT" "$SIMMANAGER_PATH" "simmanager"

# Sermanager
deploy_contract "$SERIES_MANAGER_CONTRACT" "$SERMANAGER_PATH" "sermanager"

# Abmanager
deploy_contract "$ANTIBADGE_MANAGER_CONTRACT" "$ABMANAGER_PATH" "abmanager"

# Repmanager
deploy_contract "$REPUTATION_MANAGER_CONTRACT" "$REPMANAGER_PATH" "repmanager"

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

# Authority
deploy_contract "$AUTHORITY_CONTRACT" "$AUTHORITY_PATH" "authority"

# Tap
deploy_contract "$TAP_CONTRACT" "$TAP_PATH" "tap"

# Checks
deploy_contract "$CHECKS_CONTRACT" "$CHECKS_PATH" "checks"


