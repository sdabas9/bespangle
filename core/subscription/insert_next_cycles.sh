#!/bin/bash

# Check for required arguments
if [ "$#" -ne 4 ]; then
    echo "Usage: ./insert_next_cycles.sh <contract_account> <user_account> <number_of_records> <cycle_length_in_seconds>"
    exit 1
fi

NODE_URL="http://jungle4.cryptolions.io"  # URL of the EOSIO node to use
CONTRACT=$1       # Contract account name
USER=$2           # User account name (with permission to push actions)
NUM_RECORDS=$3    # Number of records to be inserted
CYCLE_LENGTH=$4   # Cycle length in seconds

# Get the current time as the starting end time for the first cycle
LAST_END_TIME=$(date +%s)

# Insert subsequent records using 'nextcycle'
for (( i=1; i<=NUM_RECORDS; i++ )); do
    # Calculate the next end time by adding the cycle length to the last end time
    NEXT_END_TIME=$(($LAST_END_TIME + $CYCLE_LENGTH))
    NEXT_END_TIME_FORMATTED=$(date -r $NEXT_END_TIME +"%Y-%m-%dT%H:%M:%S")

    # Run cleos command to call the 'nextcycle' action
    cleos -u $NODE_URL push action $CONTRACT nextcycle '["'$NEXT_END_TIME_FORMATTED'"]' -p $USER@active

    # Update LAST_END_TIME for the next iteration
    LAST_END_TIME=$NEXT_END_TIME

    # Optional: Add a small delay if needed to avoid rapid-fire transactions
    sleep 0.5
done

