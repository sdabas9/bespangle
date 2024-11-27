#!/bin/bash

# Check for required arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: ./run_transactions.sh <contract_account> <user_account>"
    exit 1
fi

CONTRACT=$1       # Contract account name
USER=$2           # User account name (with permission to push actions)

# Base URL for cleos
NODE_URL="http://jungle4.cryptolions.io"

# List of cleos commands to run
cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "setramrate",
      "data": {
        "per_byte_cost": {
          "contract": "eosio.token",
          "quantity": "0.0001 EOS"
        }
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "setvt",
      "data": {
        "vt_percent": 5000
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "freemembers",
      "data": {
        "free_members": 3
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "freememcycle",
      "data": {
        "free_cycles": 1
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "freeseries",
      "data": {
        "free_series": 1
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "freesercycle",
      "data": {
        "free_cycles": 1
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "setmemcost",
      "data": {
        "first_additional_member_admin_cost": {
          "contract": "eosio.token",
          "quantity": "1.0000 EOS"
        }
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "setsercost",
      "data": {
        "additonal_series_admin_cost": {
          "contract": "eosio.token",
          "quantity": "10.0000 EOS"
        }
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u $NODE_URL push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "setduetime",
      "data": {
        "due_grace_seconds": 30
      },
      "authorization": [
        {
          "actor": "'"$USER"'",
          "permission": "active"
        }
      ]
    }
  ]
}'

