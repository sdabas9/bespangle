#!/bin/sh

CLEOS_URL="http://jungle4.cryptolions.io"

config_file="./accounts.txt"

# Check if the configuration file exists
if [ ! -f "$config_file" ]; then
    echo "Error: Configuration file not found at '$config_file'"
    exit 2
fi

# Source the configuration file to load variables
. "$config_file"

# Define paths

SIMPLE_MANAGER_CONTRACT_PATH="interface/simmanager"
SIMPLE_VALIDATION_CONTRACT_PATH="validations/simpleval"

MUTUAL_RECOGNITION_MANAGER_CONTRACT_PATH="interface/mrmanager"
MUTUAL_RECOGNITION_VALIDATION_CONTRACT_PATH="validations/mrval"

ORCHESTRATOR_MANAGER_CONTRACT_PATH="interface/orchmanager"
ORCHESTRATOR_VALIDATION_CONTRACT_PATH="validations/orchval"

ANDEMITTER_MANAGER_CONTRACT_PATH="interface/aemanager"
ANDEMITTER_VALIDATION_CONTRACT_PATH="validations/aeval"

BOUNDED_AGG_MANAGER_CONTRACT_PATH="interface/bamanager"
BOUNDED_AGG_VALIDATION_CONTRACT_PATH="validations/baval"

GIVER_REP_MANAGER_CONTRACT_PATH="interface/grmanager"
GIVER_REP_VALIDATION_CONTRACT_PATH="validations/grval"

BOUNDED_HLL_MANAGER_CONTRACT_PATH="interface/bhllmanager"
BOUNDED_HLL_VALIDATION_CONTRACT_PATH="validations/bhllval"

HLL_EMITTER_MANAGER_CONTRACT_PATH="interface/hllmanager"
HLL_EMITTER_VALIDATION_CONTRACT_PATH="validations/hllval"

ORG_CONTRACT_PATH="org"
AUTHORITY_CONTRACT_PATH="core/authority"

SIMPLEBADGE_CONTRACT_PATH="producers/simplebadge"
MUTUAL_RECOGNITION_CONTRACT_PATH="producers/mrbadge"

ORCHESTRATOR_CONTRACT_PATH="core/metadata"

CUMULATIVE_CONTRACT_PATH="consumers/cumulative"
STATISTICS_CONTRACT_PATH="consumers/statistics"

ANDEMITTER_CONTRACT_PATH="consumers/andemitter"

BOUNDED_AGG_CONTRACT_PATH="consumers/boundagg"
BOUNDED_STATS_CONTRACT_PATH="consumers/boundedstats"

HLL_EMITTER_CONTRACT_PATH="consumers/hllemitter"
GIVER_REP_CONTRACT_PATH="reputation/giverrep"
BOUNDED_HLL_CONTRACT_PATH="consumers/boundedhll"

SUBSCRIPTION_CONTRACT_PATH="core/subscription"

REQUESTS_CONTRACT_PATH="core/requests"
BOUNTIES_CONTRACT_PATH="core/bounties"

# Initialize the command part string
D_PART=""

# Read each line from the file
while IFS='=' read -r var_name var_value; do
    # Trim whitespace and remove potential quotes around the variable value
    var_value=$(echo $var_value | tr -d '"' | xargs)
    
    # Append to D_PART if the variable name and value are not empty
    if [ -n "$var_name" ] && [ -n "$var_value" ]; then
        D_PART+="-D${var_name}=${var_value} "
    fi
done < "$config_file"

D_PART=$(echo $D_PART | xargs)

echo $D_PART


_D_PART="-DSIMPLE_MANAGER_CONTRACT=$SIMPLE_MANAGER_CONTRACT -DSIMPLE_VALIDATION_CONTRACT=$SIMPLE_VALIDATION_CONTRACT -DMUTUAL_RECOGNITION_MANAGER_CONTRACT=$MUTUAL_RECOGNITION_MANAGER_CONTRACT -DMUTUAL_RECOGNITION_VALIDATION_CONTRACT=$MUTUAL_RECOGNITION_VALIDATION_CONTRACT -DORCHESTRATOR_MANAGER_CONTRACT=$ORCHESTRATOR_MANAGER_CONTRACT -DORCHESTRATOR_VALIDATION_CONTRACT=$ORCHESTRATOR_VALIDATION_CONTRACT
-DANDEMITTER_MANAGER_CONTRACT=$ANDEMITTER_MANAGER_CONTRACT -DANDEMITTER_VALIDATION_CONTRACT=$ANDEMITTER_VALIDATION_CONTRACT
-DBOUNDED_AGG_MANAGER_CONTRACT=$BOUNDED_AGG_MANAGER_CONTRACT -DBOUNDED_AGG_VALIDATION_CONTRACT=$BOUNDED_AGG_VALIDATION_CONTRACT -DGIVER_REP_MANAGER_CONTRACT=$GIVER_REP_MANAGER_CONTRACT -DGIVER_REP_VALIDATION_CONTRACT=$GIVER_REP_VALIDATION_CONTRACT -DBOUNDED_HLL_MANAGER_CONTRACT=$BOUNDED_HLL_MANAGER_CONTRACT -DBOUNDED_HLL_VALIDATION_CONTRACT=$BOUNDED_HLL_VALIDATION_CONTRACT
-DHLL_EMITTER_MANAGER_CONTRACT=$HLL_EMITTER_MANAGER_CONTRACT -DHLL_EMITTER_VALIDATION_CONTRACT=$HLL_EMITTER_VALIDATION_CONTRACT
-DORG_CONTRACT=$ORG_CONTRACT -DAUTHORITY_CONTRACT=$AUTHORITY_CONTRACT -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT
-DMUTUAL_RECOGNITION_CONTRACT=$MUTUAL_RECOGNITION_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DSTATISTICS_CONTRACT=$STATISTICS_CONTRACT -DANDEMITTER_CONTRACT=$ANDEMITTER_CONTRACT -DBOUNDED_AGG_CONTRACT=$BOUNDED_AGG_CONTRACT -DBOUNDED_STATS_CONTRACT=$BOUNDED_STATS_CONTRACT -DHLL_EMITTER_CONTRACT=$HLL_EMITTER_CONTRACT -DGIVER_REP_CONTRACT=$GIVER_REP_CONTRACT -DBOUNDED_HLL_CONTRACT=$BOUNDED_HLL_CONTRACT -DSUBSCRIPTION_CONTRACT=$SUBSCRIPTION_CONTRACT -DREQUESTS_CONTRACT=$REQUESTS_CONTRACT -DBOUNTIES_CONTRACT=$BOUNTIES_CONTRACT"

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

build_contract "simmanager" $SIMPLE_MANAGER_CONTRACT_PATH "$_D_PART"
build_contract "simpleval" $SIMPLE_VALIDATION_CONTRACT_PATH "$_D_PART"

build_contract "mrmanager" $MUTUAL_RECOGNITION_MANAGER_CONTRACT_PATH "$_D_PART"
build_contract "mrval" $MUTUAL_RECOGNITION_VALIDATION_CONTRACT_PATH "$_D_PART"

build_contract "orchmanager" $ORCHESTRATOR_MANAGER_CONTRACT_PATH "$_D_PART"
build_contract "orchval" $ORCHESTRATOR_VALIDATION_CONTRACT_PATH "$_D_PART"

build_contract "aemanager" $ANDEMITTER_MANAGER_CONTRACT_PATH "$_D_PART"
build_contract "aeval" $ANDEMITTER_VALIDATION_CONTRACT_PATH "$_D_PART"

build_contract "bamanager" $BOUNDED_AGG_MANAGER_CONTRACT_PATH "$_D_PART"
build_contract "baval" $BOUNDED_AGG_VALIDATION_CONTRACT_PATH "$_D_PART"

build_contract "grmanager" $GIVER_REP_MANAGER_CONTRACT_PATH "$_D_PART"
build_contract "grval" $GIVER_REP_VALIDATION_CONTRACT_PATH "$_D_PART"

build_contract "bhllmanager" $BOUNDED_HLL_MANAGER_CONTRACT_PATH "$_D_PART"
build_contract "bhllval" $BOUNDED_HLL_VALIDATION_CONTRACT_PATH "$_D_PART"

build_contract "hllmanager" $HLL_EMITTER_MANAGER_CONTRACT_PATH "$_D_PART"
build_contract "hllval" $HLL_EMITTER_VALIDATION_CONTRACT_PATH "$_D_PART"

build_contract "org" $ORG_CONTRACT_PATH "$_D_PART"
build_contract "authority" $AUTHORITY_CONTRACT_PATH "$_D_PART"

build_contract "simplebadge" $SIMPLEBADGE_CONTRACT_PATH "$_D_PART"
build_contract "mrbadge" $MUTUAL_RECOGNITION_CONTRACT_PATH "$_D_PART"

build_contract "metadata" $ORCHESTRATOR_CONTRACT_PATH "$_D_PART"

build_contract "cumulative" $CUMULATIVE_CONTRACT_PATH "$_D_PART"
build_contract "statistics" $STATISTICS_CONTRACT_PATH "$_D_PART"

build_contract "andemitter" $ANDEMITTER_CONTRACT_PATH "$_D_PART"

build_contract "boundagg" $BOUNDED_AGG_CONTRACT_PATH "$_D_PART"
build_contract "boundedstats" $BOUNDED_STATS_CONTRACT_PATH "$_D_PART"

build_contract "hllemitter" $HLL_EMITTER_CONTRACT_PATH "$_D_PART"
build_contract "giverrep" $GIVER_REP_CONTRACT_PATH "$_D_PART"
build_contract "boundedhll" $BOUNDED_HLL_CONTRACT_PATH "$_D_PART"

build_contract "subscription" $SUBSCRIPTION_CONTRACT_PATH "$_D_PART"
build_contract "requests" $REQUESTS_CONTRACT_PATH "$_D_PART"
build_contract "bounties" $BOUNTIES_CONTRACT_PATH "$_D_PART"

# simmanager
deploy_contract "$SIMPLE_MANAGER_CONTRACT" "$SIMPLE_MANAGER_CONTRACT_PATH" "simmanager"

# simpleval
deploy_contract "$SIMPLE_VALIDATION_CONTRACT" "$SIMPLE_VALIDATION_CONTRACT_PATH" "simpleval"

# mrmanager
deploy_contract "$MUTUAL_RECOGNITION_MANAGER_CONTRACT" "$MUTUAL_RECOGNITION_MANAGER_CONTRACT_PATH" "mrmanager"

# mrval
deploy_contract "$MUTUAL_RECOGNITION_VALIDATION_CONTRACT" "$MUTUAL_RECOGNITION_VALIDATION_CONTRACT_PATH" "mrval"

# orchmanager
deploy_contract "$ORCHESTRATOR_MANAGER_CONTRACT" "$ORCHESTRATOR_MANAGER_CONTRACT_PATH" "orchmanager"

# orchval
deploy_contract "$ORCHESTRATOR_VALIDATION_CONTRACT" "$ORCHESTRATOR_VALIDATION_CONTRACT_PATH" "orchval"

# aemanager
deploy_contract "$ANDEMITTER_MANAGER_CONTRACT" "$ANDEMITTER_MANAGER_CONTRACT_PATH" "aemanager"

# aeval
deploy_contract "$ANDEMITTER_VALIDATION_CONTRACT" "$ANDEMITTER_VALIDATION_CONTRACT_PATH" "aeval"

# bamanager
deploy_contract "$BOUNDED_AGG_MANAGER_CONTRACT" "$BOUNDED_AGG_MANAGER_CONTRACT_PATH" "bamanager"

# baval
deploy_contract "$BOUNDED_AGG_VALIDATION_CONTRACT" "$BOUNDED_AGG_VALIDATION_CONTRACT_PATH" "baval"

# grmanager
deploy_contract "$GIVER_REP_MANAGER_CONTRACT" "$GIVER_REP_MANAGER_CONTRACT_PATH" "grmanager"

# grval
deploy_contract "$GIVER_REP_VALIDATION_CONTRACT" "$GIVER_REP_VALIDATION_CONTRACT_PATH" "grval"

# bhllmanager
deploy_contract "$BOUNDED_HLL_MANAGER_CONTRACT" "$BOUNDED_HLL_MANAGER_CONTRACT_PATH" "bhllmanager"

# bhllval
deploy_contract "$BOUNDED_HLL_VALIDATION_CONTRACT" "$BOUNDED_HLL_VALIDATION_CONTRACT_PATH" "bhllval"

# hllmanager
deploy_contract "$HLL_EMITTER_MANAGER_CONTRACT" "$HLL_EMITTER_MANAGER_CONTRACT_PATH" "hllmanager"

# hllval
deploy_contract "$HLL_EMITTER_VALIDATION_CONTRACT" "$HLL_EMITTER_VALIDATION_CONTRACT_PATH" "hllval"

# org
deploy_contract "$ORG_CONTRACT" "$ORG_CONTRACT_PATH" "org"

# authority
deploy_contract "$AUTHORITY_CONTRACT" "$AUTHORITY_CONTRACT_PATH" "authority"

# simplebadge
deploy_contract "$SIMPLEBADGE_CONTRACT" "$SIMPLEBADGE_CONTRACT_PATH" "simplebadge"

# mrbadge
deploy_contract "$MUTUAL_RECOGNITION_CONTRACT" "$MUTUAL_RECOGNITION_CONTRACT_PATH" "mrbadge"

# metadata
deploy_contract "$ORCHESTRATOR_CONTRACT" "$ORCHESTRATOR_CONTRACT_PATH" "metadata"

# cumulative
deploy_contract "$CUMULATIVE_CONTRACT" "$CUMULATIVE_CONTRACT_PATH" "cumulative"

# statistics
deploy_contract "$STATISTICS_CONTRACT" "$STATISTICS_CONTRACT_PATH" "statistics"

# andemitter
deploy_contract "$ANDEMITTER_CONTRACT" "$ANDEMITTER_CONTRACT_PATH" "andemitter"

# boundagg
deploy_contract "$BOUNDED_AGG_CONTRACT" "$BOUNDED_AGG_CONTRACT_PATH" "boundagg"

# boundedstats
deploy_contract "$BOUNDED_STATS_CONTRACT" "$BOUNDED_STATS_CONTRACT_PATH" "boundedstats"

# hllemitter
deploy_contract "$HLL_EMITTER_CONTRACT" "$HLL_EMITTER_CONTRACT_PATH" "hllemitter"

# giverrep
deploy_contract "$GIVER_REP_CONTRACT" "$GIVER_REP_CONTRACT_PATH" "giverrep"

# boundedhll
deploy_contract "$BOUNDED_HLL_CONTRACT" "$BOUNDED_HLL_CONTRACT_PATH" "boundedhll"

# subscription
deploy_contract "$SUBSCRIPTION_CONTRACT" "$SUBSCRIPTION_CONTRACT_PATH" "subscription"

#requests
deploy_contract "$REQUESTS_CONTRACT" "$REQUESTS_CONTRACT_PATH" "requests"

#bounties
deploy_contract "$BOUNTIES_CONTRACT" "$BOUNTIES_CONTRACT_PATH" "bounties"
