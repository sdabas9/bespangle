#!/bin/bash

# Define the node URL
NODE_URL="http://jungle4.cryptolions.io"

# Define the accounts
accounts=(
    "orchestrator"
    "billingxxxxx"
    "simplebadgex"
    "gotchabadgex"
    "seriesbadgex"
    "antibadgexxx"
    "aabadgexxxxx"
    "aanftxxxxxxx"
    "interface111"
    "seriesval111"
    "roundmanager"
    "mrmanagerxxx"
    "boundedaggxx"
    "boundedstats"
    "boundedhllxx"
    "rounddelayed"
    "eosiotknpool"
    "redeemsbtxxx"
    "cumulativexx"
    "statisticsxx"
    "hllemitterxx"
    "giverrepxxxx"
    "andemitterxx"
    "notification"
    "mrnotifyxxxx"
    "tapxxxxxxxxx"
    "checksxxxxxx"
    "simmanagerxx"
    "simnotifyxxx"
    "sermanagerxx"
    "simnotifyxxx"
    "abnotifyxxxx"
    "abmanagerxxx"
    "antibadgeval"
    "sernotifyxxx"
    "simplevalxxx"
    "gotchavalxxx"
    "authorityxxx"
    "repmanagerxx"
    "repvalxxxxxx"
    "sachitsachit"
    "testuserxxxx"
    "testuseryyyy"
    "testorgxxxxx"
)

# Loop through each account and power up
for account in "${accounts[@]}"; do
    cleos -u $NODE_URL push action eosio powerup "[$account, $account, 1, 100000000000, 100000000000, \"1.0000 EOS\"]" -p $account
done

