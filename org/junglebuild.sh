#!/bin/sh

SIMPLEBADGE_CONTRACT="simplebadge2"
GOTCHABADGE_CONTRACT="gotchabadge1"
ORCHESTRATOR_CONTRACT="metadata2222"
AABADGE_CONTRACT="aabadge11111"
CUMULATIVE_CONTRACT="cumulative11"
ROUNDS_CONTRACT="rounds111111"
ACCOUNT_PREFERENCES_CONTRACT="userprefs111"
ORG_CONTRACT="edenbadgeint"
SERIESBADGE_CONTRACT="seriesbadge1"
GOV_WEIGHT_CONTRACT="govweight111"
TOKEN_STAKER_CONTRACT="tokenstaker1"


CLEOS_URL="http://jungle4.cryptolions.io"

cmake . -DSIMPLEBADGE_CONTRACT=$SIMPLEBADGE_CONTRACT -DGOTCHABADGE_CONTRACT=$GOTCHABADGE_CONTRACT -DORCHESTRATOR_CONTRACT=$ORCHESTRATOR_CONTRACT -DAABADGE_CONTRACT=$AABADGE_CONTRACT -DCUMULATIVE_CONTRACT=$CUMULATIVE_CONTRACT -DROUNDS_CONTRACT=$ROUNDS_CONTRACT -DACCOUNT_PREFERENCES_CONTRACT=$ACCOUNT_PREFERENCES_CONTRACT -DSERIESBADGE_CONTRACT=$SERIESBADGE_CONTRACT -DGOV_WEIGHT_CONTRACT=$GOV_WEIGHT_CONTRACT -DTOKEN_STAKER_CONTRACT=$TOKEN_STAKER_CONTRACT
eosio-cpp -abigen -I ./include -R ./resource -contract org -o org.wasm src/org.cpp
eosio-cpp -abigen -I ./include -R ./resource -contract govweight -o govweight.wasm src/govweight.cpp
eosio-cpp -abigen -I ./include -R ./resource -contract tokenstaker -o tokenstaker.wasm src/tokenstaker.cpp

cleos -u $CLEOS_URL set contract $ORG_CONTRACT . org.wasm org.abi -p $ORG_CONTRACT@active
cleos -u $CLEOS_URL set account permission $ORG_CONTRACT active --add-code -p $ORG_CONTRACT@active

cleos -u $CLEOS_URL set contract $GOV_WEIGHT_CONTRACT . govweight.wasm govweight.abi -p $GOV_WEIGHT_CONTRACT@active
cleos -u $CLEOS_URL set account permission $GOV_WEIGHT_CONTRACT active --add-code -p $GOV_WEIGHT_CONTRACT@active

cleos -u $CLEOS_URL set contract $TOKEN_STAKER_CONTRACT . tokenstaker.wasm tokenstaker.abi -p $TOKEN_STAKER_CONTRACT@active
cleos -u $CLEOS_URL set account permission $TOKEN_STAKER_CONTRACT active --add-code -p $TOKEN_STAKER_CONTRACT@active
