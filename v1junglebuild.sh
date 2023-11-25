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

# Script using paths
cd $METADATA_PATH
cmake . -DBILLING_CONTRACT=$BILLING_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract metadata -o metadata.wasm src/metadata.cpp
cd ../..

cd $ORGBILL_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract orgbill -o orgbill.wasm src/orgbill.cpp
cd ../..

cd $SIMPLEBADGE_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract simplebadge -o simplebadge.wasm src/simplebadge.cpp
cd ../..

cd $GOTCHABADGE_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract gotchabadge -o gotchabadge.wasm src/gotchabadge.cpp
cd ../..

cd $SERIESBADGE_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract seriesbadge -o seriesbadge.wasm src/seriesbadge.cpp
cd ../..

cd $ANTIBADGE_PATH
cmake . -DCUMULATIVE_CONTRACT_NAME=$CUMULATIVE_CONTRACT -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DCHECKS_CONTRACT_NAME=$CHECKS_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract antibadge -o antibadge.wasm src/antibadge.cpp
cd ../..

cd $AABADGE_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DATOMIC_ASSETS_CONTRACT=$ATOMIC_ASSETS_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract aabadge -o aabadge.wasm src/aabadge.cpp
cd ../..

# Skipping AANFT_PATH as it is commented in the original script

cd $ORG_PATH
cmake . -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DGOTCHABADGE_CONTRACT=$GOTCHABADGE_CONTRACT -DCLAIMASSET_CONTRACT=$CLAIMASSET_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DAABADGE_CONTRACT=$AABADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DROUNDS_CONTRACT=$ROUNDS_CONTRACT -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DASYNC_CONTRACT=$ASYNC_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract org -o org.wasm src/org.cpp
cd ..

cd $SERVAL_PATH
cmake . -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract serval -o serval.wasm src/serval.cpp
cd ../..

cd $ROUNDMANAGER_PATH
cmake . -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT -DORG_CHECKS_CONTRACT_NAME=$ORG_INTERFACE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract roundmanager -o roundmanager.wasm src/roundmanager.cpp
cd ../..

cd $MRMANAGER_PATH
cmake . -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DMUTUAL_RECOGNITION_NOTIFICATION_CONTRACT=$MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT -DORG_CHECKS_CONTRACT=$ORG_INTERFACE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract mrmanager -o mrmanager.wasm src/mrmanager.cpp
cd ../..

cd $BOUNDAGG_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract boundagg -o boundagg.wasm src/boundagg.cpp
cd ../..

cd $BOUNDEDSTATS_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract boundedstats -o boundedstats.wasm src/boundedstats.cpp
cd ../..

cd $BOUNDEDHLL_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract boundedhll -o boundedhll.wasm src/boundedhll.cpp
cd ../..

cd $DEFERRED_PATH
cmake . -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract deferred -o deferred.wasm src/deferred.cpp
cd ../..

cd $EOSIOTKNPOOL_PATH
cmake . -DREDEEM_CONTRACT_NAME=$REDEEM_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract eosiotknpool -o eosiotknpool.wasm src/eosiotknpool.cpp
cd ../..

cd $REDEEM_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract redeem -o redeem.wasm src/redeem.cpp
cd ../..

cd $CUMULATIVE_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract cumulative -o cumulative.wasm src/cumulative.cpp
cd ../..

cd $STATISTICS_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DCUMULATIVE_CONTRACT_NAME=$CUMULATIVE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract statistics -o statistics.wasm src/statistics.cpp
cd ../..

cd $HLLEMITTER_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract hllemitter -o hllemitter.wasm src/hllemitter.cpp
cd ../..


cd $GIVERREP_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DSTATS_CONTRACT_NAME=$STATISTICS_CONTRACT -DCUMULATIVE_CONTRACT_NAME=$CUMULATIVE_CONTRACT  -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract giverrep -o giverrep.wasm src/giverrep.cpp
cd ../..

cd $ANDEMITTER_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DSIMPLEBADGE_CONTRACT_NAME=$SIMPLEBADGE_CONTRACT -DCLAIMASSET_CONTRACT_NAME=$CLAIMASSET_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract andemitter -o andemitter.wasm src/andemitter.cpp
cd ../..


cd $NOTIFICATION_PATH
cmake . -DORG_INTERFACE_CONTRACT_NAME=$ORG_INTERFACE_CONTRACT -DSIMPLEBADGE_CONTRACT_NAME=$SIMPLEBADGE_CONTRACT -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DANDEMITTER_CONTRACT_NAME=$ANDEMITTER_CONTRACT -DCLAIMASSET_CONTRACT_NAME=$CLAIMASSET_CONTRACT -DSERIESBADGE_CONTRACT_NAME=$SERIESBADGE_CONTRACT -DROUND_MANAGER_CONTRACT_NAME=$ROUND_MANAGER_CONTRACT -DREDEEM_CONTRACT_NAME=$REDEEM_CONTRACT -DANTIBADGE_CONTRACT_NAME=$ANTIBADGE_CONTRACT -DDEFERRED_CONTRACT_NAME=$ROUND_DEFERRED_CONTRACT -DBOUNDED_AGG_CONTRACT_NAME=$BOUNDED_AGG_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract notification -o notification.wasm src/notification.cpp
cd ../..

cd $MRNOTIFY_PATH
cmake . -DMUTUAL_RECOGNITION_CONTRACT_NAME=$GOTCHABADGE_CONTRACT -DMUTUAL_RECOGNITION_INTERFACE_CONTRACT_NAME=$ORG_INTERFACE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract mrnotify -o mrnotify.wasm src/mrnotify.cpp
cd ../..


cd $TAP_PATH
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract tap -o tap.wasm src/tap.cpp
cd ../..

cd $CHECKS_PATH
cmake . -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DBOUNDED_AGG_CONTRACT=$BOUNDED_AGG_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract checks -o checks.wasm src/checks.cpp
cd ../..





# Metadata
cleos -u $CLEOS_URL set contract $ORCHESTRATOR_CONTRACT $METADATA_PATH metadata.wasm metadata.abi -p $ORCHESTRATOR_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORCHESTRATOR_CONTRACT active --add-code -p $ORCHESTRATOR_CONTRACT@active

# Orgbill
cleos -u $CLEOS_URL set contract $BILLING_CONTRACT $ORGBILL_PATH orgbill.wasm orgbill.abi -p $BILLING_CONTRACT@active
cleos -u $CLEOS_URL set account permission $BILLING_CONTRACT active --add-code -p $BILLING_CONTRACT@active

# Simplebadge
cleos -u $CLEOS_URL set contract $SIMPLEBADGE_CONTRACT $SIMPLEBADGE_PATH simplebadge.wasm simplebadge.abi -p $SIMPLEBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SIMPLEBADGE_CONTRACT active --add-code -p $SIMPLEBADGE_CONTRACT@active

# Gotchabadge
cleos -u $CLEOS_URL set contract $GOTCHABADGE_CONTRACT $GOTCHABADGE_PATH gotchabadge.wasm gotchabadge.abi -p $GOTCHABADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $GOTCHABADGE_CONTRACT active --add-code -p $GOTCHABADGE_CONTRACT@active

# Seriesbadge
cleos -u $CLEOS_URL set contract $SERIESBADGE_CONTRACT $SERIESBADGE_PATH seriesbadge.wasm seriesbadge.abi -p $SERIESBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SERIESBADGE_CONTRACT active --add-code -p $SERIESBADGE_CONTRACT@active

# Antibadge
cleos -u $CLEOS_URL set contract $ANTIBADGE_CONTRACT $ANTIBADGE_PATH antibadge.wasm antibadge.abi -p $ANTIBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ANTIBADGE_CONTRACT active --add-code -p $ANTIBADGE_CONTRACT@active

# Aabadge
cleos -u $CLEOS_URL set contract $AABADGE_CONTRACT $AABADGE_PATH aabadge.wasm aabadge.abi -p $AABADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $AABADGE_CONTRACT active --add-code -p $AABADGE_CONTRACT@active

# Org
cleos -u $CLEOS_URL set contract $ORG_INTERFACE_CONTRACT $ORG_PATH org.wasm org.abi -p $ORG_INTERFACE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORG_INTERFACE_CONTRACT active --add-code -p $ORG_INTERFACE_CONTRACT@active

# Serval
cleos -u $CLEOS_URL set contract $SERIES_VALIDATION_CONTRACT $SERVAL_PATH serval.wasm serval.abi -p $SERIES_VALIDATION_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SERIES_VALIDATION_CONTRACT active --add-code -p $SERIES_VALIDATION_CONTRACT@active

# Roundmanager
cleos -u $CLEOS_URL set contract $ROUND_MANAGER_CONTRACT $ROUND_MANAGER_PATH roundmanager.wasm roundmanager.abi -p $ROUND_MANAGER_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ROUND_MANAGER_CONTRACT active --add-code -p $ROUND_MANAGER_CONTRACT@active

# Mrmanager
cleos -u $CLEOS_URL set contract $MUTUAL_RECOGNITION_MANAGER_CONTRACT $MR_MANAGER_PATH mrmanager.wasm mrmanager.abi -p $MUTUAL_RECOGNITION_MANAGER_CONTRACT@active
cleos -u $CLEOS_URL set account permission $MUTUAL_RECOGNITION_MANAGER_CONTRACT active --add-code -p $MUTUAL_RECOGNITION_MANAGER_CONTRACT@active

# Boundagg
cleos -u $CLEOS_URL set contract $BOUND_AGG_CONTRACT $BOUND_AGG_PATH boundagg.wasm boundagg.abi -p $BOUND_AGG_CONTRACT@active
cleos -u $CLEOS_URL set account permission $BOUND_AGG_CONTRACT active --add-code -p $BOUND_AGG_CONTRACT@active

# Boundedstats
cleos -u $CLEOS_URL set contract $BOUNDED_STATS_CONTRACT $BOUNDED_STATS_PATH boundedstats.wasm boundedstats.abi -p $BOUNDED_STATS_CONTRACT@active
cleos -u $CLEOS_URL set account permission $BOUNDED_STATS_CONTRACT active --add-code -p $BOUNDED_STATS_CONTRACT@active

# Boundedhll
cleos -u $CLEOS_URL set contract $BOUNDED_HLL_CONTRACT $BOUNDED_HLL_PATH boundedhll.wasm boundedhll.abi -p $BOUNDED_HLL_CONTRACT@active
cleos -u $CLEOS_URL set account permission $BOUNDED_HLL_CONTRACT active --add-code -p $BOUNDED_HLL_CONTRACT@active

# Deferred
cleos -u $CLEOS_URL set contract $ROUND_DEFERRED_CONTRACT $DEFERRED_PATH deferred.wasm deferred.abi -p $ROUND_DEFERRED_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ROUND_DEFERRED_CONTRACT active --add-code -p $ROUND_DEFERRED_CONTRACT@active

# Eosiotknpool
cleos -u $CLEOS_URL set contract $EOSIO_TOKEN_POOL_CONTRACT $EOSIOTKNPOOL_PATH eosiotknpool.wasm eosiotknpool.abi -p $EOSIO_TOKEN_POOL_CONTRACT@active
cleos -u $CLEOS_URL set account permission $EOSIO_TOKEN_POOL_CONTRACT active --add-code -p $EOSIO_TOKEN_POOL_CONTRACT@active

# Redeem
cleos -u $CLEOS_URL set contract $REDEEM_CONTRACT $REDEEM_PATH redeem.wasm redeem.abi -p $REDEEM_CONTRACT@active
cleos -u $CLEOS_URL set account permission $REDEEM_CONTRACT active --add-code -p $REDEEM_CONTRACT@active

# Cumulative
cleos -u $CLEOS_URL set contract $CUMULATIVE_CONTRACT $CUMULATIVE_PATH cumulative.wasm cumulative.abi -p $CUMULATIVE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $CUMULATIVE_CONTRACT active --add-code -p $CUMULATIVE_CONTRACT@active

# Statistics
cleos -u $CLEOS_URL set contract $STATISTICS_CONTRACT $STATISTICS_PATH statistics.wasm statistics.abi -p $STATISTICS_CONTRACT@active
cleos -u $CLEOS_URL set account permission $STATISTICS_CONTRACT active --add-code -p $STATISTICS_CONTRACT@active

# Hllemitter
cleos -u $CLEOS_URL set contract $HLL_EMITTER_CONTRACT $HLLEMITTER_PATH hllemitter.wasm hllemitter.abi -p $HLL_EMITTER_CONTRACT@active
cleos -u $CLEOS_URL set account permission $HLL_EMITTER_CONTRACT active --add-code -p $HLL_EMITTER_CONTRACT@active

# Giverrep
cleos -u $CLEOS_URL set contract $GIVER_REP_CONTRACT $GIVERREP_PATH giverrep.wasm giverrep.abi -p $GIVER_REP_CONTRACT@active
cleos -u $CLEOS_URL set account permission $GIVER_REP_CONTRACT active --add-code -p $GIVER_REP_CONTRACT@active

# Andemitter
cleos -u $CLEOS_URL set contract $ANDEMITTER_CONTRACT $ANDEMITTER_PATH andemitter.wasm andemitter.abi -p $ANDEMITTER_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ANDEMITTER_CONTRACT active --add-code -p $ANDEMITTER_CONTRACT@active

# Notification
cleos -u $CLEOS_URL set contract $NOTIFICATION_CONTRACT $NOTIFICATION_PATH notification.wasm notification.abi -p $NOTIFICATION_CONTRACT@active
cleos -u $CLEOS_URL set account permission $NOTIFICATION_CONTRACT active --add-code -p $NOTIFICATION_CONTRACT@active

# Mrnotify
cleos -u $CLEOS_URL set contract $MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT $MRNOTIFY_PATH mrnotify.wasm mrnotify.abi -p $MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT@active
cleos -u $CLEOS_URL set account permission $MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT active --add-code -p $MUTUAL_RECOGNITION_NOTIFICATION_CONTRACT@active

# Tap
cleos -u $CLEOS_URL set contract $TAP_CONTRACT $TAP_PATH tap.wasm tap.abi -p $TAP_CONTRACT@active
cleos -u $CLEOS_URL set account permission $TAP_CONTRACT active --add-code -p $TAP_CONTRACT@active

# Checks
cleos -u $CLEOS_URL set contract $CHECKS_CONTRACT $CHECKS_PATH checks.wasm checks.abi -p $CHECKS_CONTRACT@active
cleos -u $CLEOS_URL set account permission $CHECKS_CONTRACT active --add-code -p $CHECKS_CONTRACT@active



