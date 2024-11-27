#!/bin/bash

# Check for required arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: ./insert_billcycle.sh <contract_account> <user_account> <number_of_records> <cycle_length_in_seconds>"
    exit 1
fi

CONTRACT=$1       # Contract account name
USER=$2           # User account name (with permission to push actions)
NUM_RECORDS=$3    # Number of records to be inserted
CYCLE_LENGTH=$4   # Cycle length in seconds

# Get the current time as the start time for the first record
START_TIME=$(date +%Y-%m-%dT%H:%M:%S)
END_TIME=$(date -d "$START_TIME + $CYCLE_LENGTH seconds" +%Y-%m-%dT%H:%M:%S)

# Insert the first record using 'firstcycle'
cleos push action $CONTRACT firstcycle '["'$START_TIME'", "'$END_TIME'"]' -p $USER@active

# Insert subsequent records using 'nextcycle'
for (( i=2; i<=NUM_RECORDS; i++ )); do
    # Calculate the next end_time by adding the cycle length to the previous end_time
    END_TIME=$(date -d "$END_TIME + $CYCLE_LENGTH seconds" +%Y-%m-%dT%H:%M:%S)

    # Run cleos command to call the 'nextcycle' action
    cleos push action $CONTRACT nextcycle '["'$END_TIME'"]' -p $USER@active

    # Optional: Add a small delay if needed to avoid rapid-fire transactions
    sleep 0.5
done

