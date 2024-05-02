#!/bin/bash

# Check if the correct number of arguments is given
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 creator_account accounts_file"
    exit 1
fi

# Assign command line arguments to variables
creator_account=$1
accounts_file=$2

# Check if the accounts file exists
if [ ! -f "$accounts_file" ]; then
    echo "Error: File '$accounts_file' not found."
    exit 2
fi

# Define the public key for owner and active permissions
public_key="EOS8VLRh9JdutT2DFa3zqofQk1h1AYgA9NkjLBuPNpgGHJnxu1XSx"

# Define the resources to allocate
stake_net="0.1 EOS" # Stake 0.1 EOS for NET
stake_cpu="0.1 EOS" # Stake 0.1 EOS for CPU
ram_bytes=8192      # Buy 8192 bytes of RAM

# Read each line from the file
while IFS='=' read -r var_name account_name; do
    # Trim whitespace and remove potential quotes around the account name
    account_name=$(echo $account_name | tr -d '"' | xargs)

    # Skip empty account names
    if [ -z "$account_name" ]; then
        continue
    fi

    echo "Creating account: $account_name"
    cleos -u http://jungle4.cryptolions.io system newaccount --stake-net "$stake_net" --stake-cpu "$stake_cpu" --buy-ram-bytes "$ram_bytes" \
        "$creator_account" "$account_name" "$public_key" "$public_key"
    if [ $? -eq 0 ]; then
        echo "Account '$account_name' created successfully."
    else
        echo "Failed to create account '$account_name'."
    fi
done < "$accounts_file"

echo "Finished processing all accounts."

