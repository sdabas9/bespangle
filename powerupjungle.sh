#!/bin/bash

# Define the node URL
NODE_URL="http://jungle4.cryptolions.io"

# Define the accounts
accounts=(
    "simmanagerzz"
    "orgzzzzzzzzz"
    "simplevalzzz"
    "cumulativezz"
    "statisticszz"
    "orchzzzzzzzz"
    "authorityzzz"
    "simplebadgez"
)

# Loop through each account and power up
for account in "${accounts[@]}"; do
    cleos -u $NODE_URL push action eosio powerup "[$account, $account, 1, 100000000000, 100000000000, \"1.0000 EOS\"]" -p $account
done

