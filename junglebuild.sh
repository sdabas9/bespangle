#!/bin/sh

ORCHESTRATOR_CONTRACT="router111111"
ACCOUNT_PREFERENCES_CONTRACT="userprefs111"
BILLING_CONTRACT="billing11111"

SIMPLEBADGE_CONTRACT="basicissue11"
GOTCHABADGE_CONTRACT="gotchabadge1"
SERIESBADGE_CONTRACT="seriesbadge1"
ANTIBADGE="antibadge111"

AABADGE_CONTRACT="aabadge11111"
ATOMIC_ASSETS_CONTRACT="atomicassets"
ROUNDS_CONTRACT="rounds111111"
CUMULATIVE_CONTRACT="accounting11"
AANFT_CONTRACT="aanft2222222"
ORG_INTERFACE_CONTRACT="interface111"
ASYNC_CONTRACT="async1111111"
ANDEMITTER_CONTRACT="andemitter11"
CLAIMASSET_CONTRACT="claimasset11"
NOTIFICATION_CONTRACT="notification"
TAP_CONTRACT="tap111111111"
CHECKS_CONTRACT="checks111111"
STATISTICS="statistics13"
SERIESVALIDATION="seriesval111"
BOUNDAGG_CONTRACT="boundagg1111"

CLEOS_URL="http://jungle4.cryptolions.io"

cd core/metadata
cmake . -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT 
eosio-cpp -abigen -I ./include -R ./resource -contract metadata -o metadata.wasm src/metadata.cpp
cd ../..


cd core/orgbill
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract orgbill -o orgbill.wasm src/orgbill.cpp
cd ../..

cd core/notification
cmake . -DORG_INTERFACE_CONTRACT_NAME=$ORG_INTERFACE_CONTRACT -DSIMPLEBADGE_CONTRACT_NAME=$SIMPLEBADGE_CONTRACT -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DANDEMITTER_CONTRACT_NAME=$ANDEMITTER_CONTRACT -DCLAIMASSET_CONTRACT_NAME=$CLAIMASSET_CONTRACT -DSERIESBADGE_CONTRACT_NAME=$SERIESBADGE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract notification -o notification.wasm src/notification.cpp
cd ../..

cd core/checks
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DSIMPLEBADGE_CONTRACT_NAME=$SIMPLEBADGE_CONTRACT -DCUMULATIVE_CONTRACT_NAME=$CUMULATIVE_CONTRACT -DSERIESBADGE_CONTRACT_NAME=$SERIESBADGE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract checks -o checks.wasm src/checks.cpp
cd ../..

cd producers/simplebadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract simplebadge -o simplebadge.wasm src/simplebadge.cpp
cd ../..

#cd producers/gotchabadge
#cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
#eosio-cpp -abigen -I ./include -R ./resource -contract gotchabadge -o gotchabadge.wasm src/gotchabadge.cpp
#cd ../..

cd producers/seriesbadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract seriesbadge -o seriesbadge.wasm src/seriesbadge.cpp
cd ../..

cd producers/antibadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DCHECKS_CONTRACT_NAME=$CHECKS_CONTRACT 
eosio-cpp -abigen -I ./include -R ./resource -contract antibadge -o antibadge.wasm src/antibadge.cpp
cd ../..

#cd producers/claimasset
#cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
#eosio-cpp -abigen -I ./include -R ./resource -contract claimasset -o claimasset.wasm src/claimasset.cpp
#cd ../..

cd consumers/cumulative
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract cumulative -o cumulative.wasm src/cumulative.cpp
cd ../..

cd consumers/boundagg
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract boundagg -o boundagg.wasm src/boundagg.cpp
cd ../..


#cd consumers/rounds
#cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
#eosio-cpp -abigen -I ./include -R ./resource -contract rounds -o rounds.wasm src/rounds.cpp
#cd ../..

cd consumers/aabadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DATOMIC_ASSETS_CONTRACT=$ATOMIC_ASSETS_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract aabadge -o aabadge.wasm src/aabadge.cpp
cd ../..

#cd consumers/aanft
#cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DATOMIC_ASSETS_CONTRACT=$ATOMIC_ASSETS_CONTRACT
#eosio-cpp -abigen -I ./include -R ./resource -contract aanft -o aanft.wasm src/aanft.cpp
#cd ../..

cd consumers/andemitter
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DSIMPLEBADGE_CONTRACT_NAME=$SIMPLEBADGE_CONTRACT -DCLAIMASSET_CONTRACT_NAME=$CLAIMASSET_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract andemitter -o andemitter.wasm src/andemitter.cpp
cd ../..

cd consumers/tap
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract tap -o tap.wasm src/tap.cpp
cd ../..

cd consumers/statistics
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DCUMULATIVE_CONTRACT_NAME=$CUMULATIVE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract statistics -o statistics.wasm src/statistics.cpp
cd ../..

cd org
cmake . -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DGOTCHABADGE_CONTRACT=$GOTCHABADGE_CONTRACT -DCLAIMASSET_CONTRACT=$CLAIMASSET_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DAABADGE_CONTRACT=$AABADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DROUNDS_CONTRACT=$ROUNDS_CONTRACT -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DASYNC_CONTRACT=$ASYNC_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract org -o org.wasm src/org.cpp
cd ..

#cd async
#cmake . -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT
#eosio-cpp -abigen -I ./include -R ./resource -contract async -o async.wasm src/async.cpp
#cd ..

cd validations/serval
cmake . -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT 
eosio-cpp -abigen -I ./include -R ./resource -contract serval -o serval.wasm src/serval.cpp
cd ../..

cleos -u $CLEOS_URL set contract $ORCHESTRATOR_CONTRACT core/metadata metadata.wasm metadata.abi -p $ORCHESTRATOR_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORCHESTRATOR_CONTRACT active --add-code -p $ORCHESTRATOR_CONTRACT@active

#cleos -u $CLEOS_URL set contract $ACCOUNT_PREFERENCES_CONTRACT core/userprefs userprefs.wasm userprefs.abi -p $ACCOUNT_PREFERENCES_CONTRACT@active
#cleos -u $CLEOS_URL set account permission $ACCOUNT_PREFERENCES_CONTRACT active --add-code -p $ACCOUNT_PREFERENCES_CONTRACT@active

cleos -u $CLEOS_URL set contract $BILLING_CONTRACT core/orgbill orgbill.wasm orgbill.abi -p $BILLING_CONTRACT@active
cleos -u $CLEOS_URL set account permission $BILLING_CONTRACT active --add-code -p $BILLING_CONTRACT@active

cleos -u $CLEOS_URL set contract $SIMPLEBADGE_CONTRACT producers/simplebadge simplebadge.wasm simplebadge.abi -p $SIMPLEBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SIMPLEBADGE_CONTRACT active --add-code -p $SIMPLEBADGE_CONTRACT@active

#cleos -u $CLEOS_URL set contract $GOTCHABADGE_CONTRACT producers/gotchabadge gotchabadge.wasm gotchabadge.abi -p $GOTCHABADGE_CONTRACT@active
#cleos -u $CLEOS_URL set account permission $GOTCHABADGE_CONTRACT active --add-code -p $GOTCHABADGE_CONTRACT@active

cleos -u $CLEOS_URL set contract $SERIESBADGE_CONTRACT producers/seriesbadge seriesbadge.wasm seriesbadge.abi -p $SERIESBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SERIESBADGE_CONTRACT active --add-code -p $SERIESBADGE_CONTRACT@active

cleos -u $CLEOS_URL set contract $AABADGE_CONTRACT consumers/aabadge aabadge.wasm aabadge.abi -p $AABADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $AABADGE_CONTRACT active --add-code -p $AABADGE_CONTRACT@active

#cleos -u $CLEOS_URL set contract $AANFT_CONTRACT consumers/aanft aanft.wasm aanft.abi -p $AANFT_CONTRACT@active
#cleos -u $CLEOS_URL set account permission $AANFT_CONTRACT active --add-code -p $AANFT_CONTRACT@active

cleos -u $CLEOS_URL set contract $CUMULATIVE_CONTRACT consumers/cumulative cumulative.wasm cumulative.abi -p $CUMULATIVE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $CUMULATIVE_CONTRACT active --add-code -p $CUMULATIVE_CONTRACT@active

#cleos -u $CLEOS_URL set contract $ROUNDS_CONTRACT consumers/rounds rounds.wasm rounds.abi -p $ROUNDS_CONTRACT@active
#cleos -u $CLEOS_URL set account permission $ROUNDS_CONTRACT active --add-code -p $ROUNDS_CONTRACT@active

cleos -u $CLEOS_URL set contract $ORG_INTERFACE_CONTRACT org org.wasm org.abi -p $ORG_INTERFACE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORG_INTERFACE_CONTRACT active --add-code -p $ORG_INTERFACE_CONTRACT@active

#cleos -u $CLEOS_URL set contract $ASYNC_CONTRACT async async.wasm async.abi -p $ASYNC_CONTRACT@active
#cleos -u $CLEOS_URL set account permission $ASYNC_CONTRACT active --add-code -p $ASYNC_CONTRACT@active

#cleos -u $CLEOS_URL set contract $CLAIMASSET_CONTRACT producers/claimasset claimasset.wasm claimasset.abi -p $CLAIMASSET_CONTRACT@active
#cleos -u $CLEOS_URL set account permission $CLAIMASSET_CONTRACT active --add-code -p $CLAIMASSET_CONTRACT@active

cleos -u $CLEOS_URL set contract $NOTIFICATION_CONTRACT core/notification notification.wasm notification.abi -p $NOTIFICATION_CONTRACT@active
cleos -u $CLEOS_URL set account permission $NOTIFICATION_CONTRACT active --add-code -p $NOTIFICATION_CONTRACT@active

cleos -u $CLEOS_URL set contract $ANDEMITTER_CONTRACT consumers/andemitter andemitter.wasm andemitter.abi -p $ANDEMITTER_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ANDEMITTER_CONTRACT active --add-code -p $ANDEMITTER_CONTRACT@active

cleos -u $CLEOS_URL set contract $STATISTICS consumers/statistics statistics.wasm statistics.abi -p $STATISTICS@active
cleos -u $CLEOS_URL set account permission $STATISTICS active --add-code -p $STATISTICS@active

cleos -u $CLEOS_URL set contract $TAP_CONTRACT consumers/tap tap.wasm tap.abi -p $TAP_CONTRACT@active
cleos -u $CLEOS_URL set account permission $TAP_CONTRACT active --add-code -p $TAP_CONTRACT@active

cleos -u $CLEOS_URL set contract $CHECKS_CONTRACT core/checks checks.wasm checks.abi -p $CHECKS_CONTRACT@active
cleos -u $CLEOS_URL set account permission $CHECKS_CONTRACT active --add-code -p $CHECKS_CONTRACT@active

cleos -u $CLEOS_URL set contract $SERIESVALIDATION validations/serval serval.wasm serval.abi -p $SERIESVALIDATION@active
cleos -u $CLEOS_URL set account permission $SERIESVALIDATION active --add-code -p $SERIESVALIDATION@active

cleos -u $CLEOS_URL set contract $ANTIBADGE producers/antibadge antibadge.wasm antibadge.abi -p $ANTIBADGE@active
cleos -u $CLEOS_URL set account permission $ANTIBADGE active --add-code -p $ANTIBADGE@active

cleos -u $CLEOS_URL set contract $BOUNDAGG_CONTRACT consumers/boundagg boundagg.wasm boundagg.abi -p $BOUNDAGG_CONTRACT@active
cleos -u $CLEOS_URL set account permission $BOUNDAGG_CONTRACT active --add-code -p $BOUNDAGG_CONTRACT@active


