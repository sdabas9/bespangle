#!/bin/sh

ORCHESTRATOR_CONTRACT="meta.rep"
BILLING_CONTRACT="billing.rep"
ACCOUNT_PREFERENCES_CONTRACT="userprefs111"

SIMPLEBADGE_CONTRACT="simplep.rep"
SERIESBADGE_CONTRACT="seriesp.rep"
GOTCHABADGE_CONTRACT="dummy"

AABADGE_CONTRACT="aabadgec.rep"
ATOMIC_ASSETS_CONTRACT="atomicassets"
CUMULATIVE_CONTRACT="aggc.rep"
ORG_INTERFACE_CONTRACT="orgint.rep"
CHECKS_CONTRACT="checks.rep"
NOTIFICATION_CONTRACT="notify.rep"
ASYNC_CONTRACT="async1111111"
ANDEMITTER_CONTRACT="andemitter11"
CLAIMASSET_CONTRACT="claimasset11"
SERIESVALIDATION="serval.rep"

CLEOS_URL="https://eos.greymass.com"

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

cd producers/seriesbadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DNOTIFICATION_CONTRACT_NAME=$NOTIFICATION_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract seriesbadge -o seriesbadge.wasm src/seriesbadge.cpp
cd ../..


cd consumers/cumulative
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract cumulative -o cumulative.wasm src/cumulative.cpp
cd ../..


cd consumers/aabadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DATOMIC_ASSETS_CONTRACT=$ATOMIC_ASSETS_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract aabadge -o aabadge.wasm src/aabadge.cpp
cd ../..

cd org
cmake . -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DNOTIFICATION_CONTRACT=$NOTIFICATION_CONTRACT -DGOTCHABADGE_CONTRACT=$GOTCHABADGE_CONTRACT -DCLAIMASSET_CONTRACT=$CLAIMASSET_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DAABADGE_CONTRACT=$AABADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DROUNDS_CONTRACT=$ROUNDS_CONTRACT -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DASYNC_CONTRACT=$ASYNC_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract org -o org.wasm src/org.cpp
cd ..

cd validations/serval
cmake . -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT 
eosio-cpp -abigen -I ./include -R ./resource -contract serval -o serval.wasm src/serval.cpp
cd ../..


cleos -u $CLEOS_URL set contract $ORCHESTRATOR_CONTRACT core/metadata metadata.wasm metadata.abi -p $ORCHESTRATOR_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORCHESTRATOR_CONTRACT active --add-code -p $ORCHESTRATOR_CONTRACT@active


cleos -u $CLEOS_URL set contract $BILLING_CONTRACT core/orgbill orgbill.wasm orgbill.abi -p $BILLING_CONTRACT@active
cleos -u $CLEOS_URL set account permission $BILLING_CONTRACT active --add-code -p $BILLING_CONTRACT@active

cleos -u $CLEOS_URL set contract $SIMPLEBADGE_CONTRACT producers/simplebadge simplebadge.wasm simplebadge.abi -p $SIMPLEBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SIMPLEBADGE_CONTRACT active --add-code -p $SIMPLEBADGE_CONTRACT@active


cleos -u $CLEOS_URL set contract $SERIESBADGE_CONTRACT producers/seriesbadge seriesbadge.wasm seriesbadge.abi -p $SERIESBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SERIESBADGE_CONTRACT active --add-code -p $SERIESBADGE_CONTRACT@active

cleos -u $CLEOS_URL set contract $AABADGE_CONTRACT consumers/aabadge aabadge.wasm aabadge.abi -p $AABADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $AABADGE_CONTRACT active --add-code -p $AABADGE_CONTRACT@active


cleos -u $CLEOS_URL set contract $CUMULATIVE_CONTRACT consumers/cumulative cumulative.wasm cumulative.abi -p $CUMULATIVE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $CUMULATIVE_CONTRACT active --add-code -p $CUMULATIVE_CONTRACT@active


cleos -u $CLEOS_URL set contract $ORG_INTERFACE_CONTRACT org org.wasm org.abi -p $ORG_INTERFACE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORG_INTERFACE_CONTRACT active --add-code -p $ORG_INTERFACE_CONTRACT@active


cleos -u $CLEOS_URL set contract $NOTIFICATION_CONTRACT core/notification notification.wasm notification.abi -p $NOTIFICATION_CONTRACT@active
cleos -u $CLEOS_URL set account permission $NOTIFICATION_CONTRACT active --add-code -p $NOTIFICATION_CONTRACT@active

cleos -u $CLEOS_URL set contract $CHECKS_CONTRACT core/checks checks.wasm checks.abi -p $CHECKS_CONTRACT@active
cleos -u $CLEOS_URL set account permission $CHECKS_CONTRACT active --add-code -p $CHECKS_CONTRACT@active

cleos -u $CLEOS_URL set contract $SERIESVALIDATION validations/serval serval.wasm serval.abi -p $SERIESVALIDATION@active
cleos -u $CLEOS_URL set account permission $SERIESVALIDATION active --add-code -p $SERIESVALIDATION@active
