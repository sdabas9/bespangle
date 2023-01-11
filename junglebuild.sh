#!/bin/sh

ORCHESTRATOR_CONTRACT="metadata2222"
ACCOUNT_PREFERENCES_CONTRACT="userprefs111"
BILLING_CONTRACT="billing11111"

SIMPLEBADGE_CONTRACT="simplebadge2"
GOTCHABADGE_CONTRACT="gotchabadge1"
SERIESBADGE_CONTRACT="seriesbadge1"

AABADGE_CONTRACT="aabadge11111"
ATOMIC_ASSETS_CONTRACT="atomicassets"
ROUNDS_CONTRACT="rounds111111"
CUMULATIVE_CONTRACT="cumulative11"
AANFT_CONTRACT="aanft2222222"
ORG_INTERFACE_CONTRACT="organization"
ASYNC_CONTRACT="async1111111"

CLEOS_URL="http://jungle4.cryptolions.io"

cd core/metadata
cmake . -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT 
eosio-cpp -abigen -I ./include -R ./resource -contract metadata -o metadata.wasm src/metadata.cpp
cd ../..

cd core/userprefs
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT 
eosio-cpp -abigen -I ./include -R ./resource -contract userprefs -o userprefs.wasm src/userprefs.cpp
cd ../..

cd core/orgbill
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract orgbill -o orgbill.wasm src/orgbill.cpp
cd ../..


cd producers/simplebadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DORG_INTERFACE_CONTRACT_NAME=$ORG_INTERFACE_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract simplebadge -o simplebadge.wasm src/simplebadge.cpp
cd ../..

cd producers/gotchabadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract gotchabadge -o gotchabadge.wasm src/gotchabadge.cpp
cd ../..

cd producers/seriesbadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract seriesbadge -o seriesbadge.wasm src/seriesbadge.cpp
cd ../..

cd consumers/cumulative
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract cumulative -o cumulative.wasm src/cumulative.cpp
cd ../..

cd consumers/rounds
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract rounds -o rounds.wasm src/rounds.cpp
cd ../..

cd consumers/aabadge
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DATOMIC_ASSETS_CONTRACT=$ATOMIC_ASSETS_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract aabadge -o aabadge.wasm src/aabadge.cpp
cd ../..

cd consumers/aanft
cmake . -DORCHESTRATOR_CONTRACT_NAME=$ORCHESTRATOR_CONTRACT -DBILLING_CONTRACT=$BILLING_CONTRACT -DATOMIC_ASSETS_CONTRACT=$ATOMIC_ASSETS_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract aanft -o aanft.wasm src/aanft.cpp
cd ../..

cd org
cmake . -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DGOTCHABADGE_CONTRACT=$GOTCHABADGE_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DAABADGE_CONTRACT=$AABADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DROUNDS_CONTRACT=$ROUNDS_CONTRACT -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DASYNC_CONTRACT=$ASYNC_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract org -o org.wasm src/org.cpp
cd ..

cd async
cmake . -DORG_INTERFACE_CONTRACT=$ORG_INTERFACE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract async -o async.wasm src/async.cpp
cd ..

cleos -u $CLEOS_URL set contract $ORCHESTRATOR_CONTRACT core/metadata metadata.wasm metadata.abi -p $ORCHESTRATOR_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORCHESTRATOR_CONTRACT active --add-code -p $ORCHESTRATOR_CONTRACT@active

cleos -u $CLEOS_URL set contract $ACCOUNT_PREFERENCES_CONTRACT core/userprefs userprefs.wasm userprefs.abi -p $ACCOUNT_PREFERENCES_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ACCOUNT_PREFERENCES_CONTRACT active --add-code -p $ACCOUNT_PREFERENCES_CONTRACT@active

cleos -u $CLEOS_URL set contract $BILLING_CONTRACT core/orgbill orgbill.wasm orgbill.abi -p $BILLING_CONTRACT@active
cleos -u $CLEOS_URL set account permission $BILLING_CONTRACT active --add-code -p $BILLING_CONTRACT@active

cleos -u $CLEOS_URL set contract $SIMPLEBADGE_CONTRACT producers/simplebadge simplebadge.wasm simplebadge.abi -p $SIMPLEBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SIMPLEBADGE_CONTRACT active --add-code -p $SIMPLEBADGE_CONTRACT@active

cleos -u $CLEOS_URL set contract $GOTCHABADGE_CONTRACT producers/gotchabadge gotchabadge.wasm gotchabadge.abi -p $GOTCHABADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $GOTCHABADGE_CONTRACT active --add-code -p $GOTCHABADGE_CONTRACT@active

cleos -u $CLEOS_URL set contract $SERIESBADGE_CONTRACT producers/seriesbadge seriesbadge.wasm seriesbadge.abi -p $SERIESBADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $SERIESBADGE_CONTRACT active --add-code -p $SERIESBADGE_CONTRACT@active

cleos -u $CLEOS_URL set contract $AABADGE_CONTRACT consumers/aabadge aabadge.wasm aabadge.abi -p $AABADGE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $AABADGE_CONTRACT active --add-code -p $AABADGE_CONTRACT@active

cleos -u $CLEOS_URL set contract $AANFT_CONTRACT consumers/aanft aanft.wasm aanft.abi -p $AANFT_CONTRACT@active
cleos -u $CLEOS_URL set account permission $AANFT_CONTRACT active --add-code -p $AANFT_CONTRACT@active

cleos -u $CLEOS_URL set contract $CUMULATIVE_CONTRACT consumers/cumulative cumulative.wasm cumulative.abi -p $CUMULATIVE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $CUMULATIVE_CONTRACT active --add-code -p $CUMULATIVE_CONTRACT@active

cleos -u $CLEOS_URL set contract $ROUNDS_CONTRACT consumers/rounds rounds.wasm rounds.abi -p $ROUNDS_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ROUNDS_CONTRACT active --add-code -p $ROUNDS_CONTRACT@active

cleos -u $CLEOS_URL set contract $ORG_INTERFACE_CONTRACT org org.wasm org.abi -p $ORG_INTERFACE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORG_INTERFACE_CONTRACT active --add-code -p $ORG_INTERFACE_CONTRACT@active

cleos -u $CLEOS_URL set contract $ASYNC_CONTRACT async async.wasm async.abi -p $ASYNC_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ASYNC_CONTRACT active --add-code -p $ASYNC_CONTRACT@active
