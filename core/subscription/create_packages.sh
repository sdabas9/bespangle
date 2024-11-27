#!/bin/bash

# Check for required arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: ./create_packages.sh <contract_account> <user_account>"
    exit 1
fi

CONTRACT=$1       # Contract account name
USER=$2           # User account name (with permission to push actions)

# Define an array of JSON data for each package
PACKAGES=(
'{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "newpack",
      "data": {
        "package": "a",
        "unique_issuance_size": 1,
        "expiry_duration_in_secs": 60,
        "ram_in_bytes": "10000 WRAM",
        "only_ram_as_payment_method": false,
        "active": true
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

'{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "newpack",
      "data": {
        "package": "b",
        "unique_issuance_size": 3,
        "expiry_duration_in_secs": 60,
        "ram_in_bytes": "20000 WRAM",
        "only_ram_as_payment_method": false,
        "active": true
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

'{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "newpack",
      "data": {
        "package": "c",
        "unique_issuance_size": 3,
        "expiry_duration_in_secs": 120,
        "ram_in_bytes": "30000 WRAM",
        "only_ram_as_payment_method": false,
        "active": true
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

'{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "'"$CONTRACT"'",
      "name": "newpack",
      "data": {
        "package": "d",
        "unique_issuance_size": 5,
        "expiry_duration_in_secs": 120,
        "ram_in_bytes": "40000 WRAM",
        "only_ram_as_payment_method": false,
        "active": true
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
)

# Iterate over the JSON data and push each transaction using cleos
for PACKAGE in "${PACKAGES[@]}"; do
    cleos -u http://jungle4.cryptolions.io push transaction "$PACKAGE"
    sleep 1  # Optional: Add delay between transactions to avoid spamming the blockchain
done

