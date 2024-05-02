#!/bin/bash

# Check if the correct number of arguments is given
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 payer accounts_file"
    exit 1
fi

# Assign command line arguments to variables
payer=$1
accounts_file=$2

# Check if the accounts file exists
if [ ! -f "$accounts_file" ]; then
    echo "Error: File '$accounts_file' not found."
    exit 2
fi

# RAM purchase amount
ram_amount="300.0000 EOS"

# Read each line from the file
while IFS='=' read -r var_name account_name; do
    # Trim whitespace and remove potential quotes around the account name
    account_name=$(echo $account_name | tr -d '"' | xargs)

    # Skip empty account names
    if [ -z "$account_name" ]; then
        continue
    fi

    echo "Buying RAM for account: $account_name"
    cleos -u http://jungle4.cryptolions.io system buyram "$payer" "$account_name" "$ram_amount"
    if [ $? -eq 0 ]; then
        echo "Successfully bought RAM for '$account_name'."
    else
        echo "Failed to buy RAM for '$account_name'."
    fi
done < "$accounts_file"

echo "Finished processing all accounts."

