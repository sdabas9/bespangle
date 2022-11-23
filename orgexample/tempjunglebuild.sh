#!/bin/sh

SIMPLEBADGE_CONTRACT="simplebadge2"
GOTCHABADGE_CONTRACT="gotchabadge1"
ORCHESTRATOR_CONTRACT="metadata2222"
AABADGE_CONTRACT="aabadge11111"
CUMULATIVE_CONTRACT="cumulative11"
ROUNDS_CONTRACT="rounds111111"
ACCOUNT_PREFERENCES_CONTRACT="userprefs111"
EDEN_ORG_INTERFACE_CONTRACT="edenbadgeint"
SERIESBADGE_CONTRACT="seriesbadge1"
EDEN_VALIDATION_CONTRACT="edenbadgeval"
ELECTIONS_CONTRACT="elections111"


CLEOS_URL="http://jungle4.cryptolions.io"

cd org
cmake . -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DGOTCHABADGE_CONTRACT=$GOTCHABADGE_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DAABADGE_CONTRACT=$AABADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DROUNDS_CONTRACT=$ROUNDS_CONTRACT -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT 
eosio-cpp -abigen -I ./include -R ./resource -contract org -o org.wasm src/org.cpp
cd ..

cd elections
cmake . -DEDEN_ORG_INTERFACE_CONTRACT=$EDEN_ORG_INTERFACE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract elections -o elections.wasm src/elections.cpp
cd ..

cd edenvalidate
cmake . -DEDEN_ORG_INTERFACE_CONTRACT=$EDEN_ORG_INTERFACE_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract edenvalidate -o edenvalidate.wasm src/edenvalidate.cpp
cd ..

cleos -u $CLEOS_URL set contract $EDEN_ORG_INTERFACE_CONTRACT org org.wasm org.abi -p $EDEN_ORG_INTERFACE_CONTRACT@active
cleos -u $CLEOS_URL set account permission $EDEN_ORG_INTERFACE_CONTRACT active --add-code -p $EDEN_ORG_INTERFACE_CONTRACT@active

cleos -u $CLEOS_URL set contract $ELECTIONS_CONTRACT elections elections.wasm elections.abi -p $ELECTIONS_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ELECTIONS_CONTRACT active --add-code -p $ELECTIONS_CONTRACT@active

cleos -u $CLEOS_URL set contract $EDEN_VALIDATION_CONTRACT edenvalidate edenvalidate.wasm edenvalidate.abi -p $EDEN_VALIDATION_CONTRACT@active
cleos -u $CLEOS_URL set account permission $EDEN_VALIDATION_CONTRACT active --add-code -p $EDEN_VALIDATION_CONTRACT@active
