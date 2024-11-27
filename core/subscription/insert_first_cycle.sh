#!/bin/bash

# Check for required arguments
if [ "$#" -ne 4 ]; then
    echo "Usage: ./insert_first_cycle.sh <contract_account> <user_account> <start_time_in_seconds> <cycle_length_in_seconds>"
    exit 1
fi

NODE_URL="http://jungle4.cryptolions.io"  # URL of the EOSIO node to use
CONTRACT=$1       # Contract account name
USER=$2           # User account name (with permission to push actions)
START_TIME=$3     # Start time in seconds (timestamp format)
CYCLE_LENGTH=$4   # Cycle length in seconds

# Convert the start time from seconds to the required date format (ISO 8601)
START_TIME_FORMATTED=$(date -r $START_TIME +"%Y-%m-%dT%H:%M:%S")
END_TIME=$(date -r $(($START_TIME + $CYCLE_LENGTH)) +"%Y-%m-%dT%H:%M:%S")

# Debugging information to check the formatted dates
echo "Start time (formatted): $START_TIME_FORMATTED"
echo "End time (formatted): $END_TIME"

# Check if the formatted dates are valid
if [ -z "$START_TIME_FORMATTED" ] || [ -z "$END_TIME" ]; then
    echo "Error: Invalid date conversion."
    exit 1
fi

# Insert the first record using 'firstcycle'
cleos -u $NODE_URL push action $CONTRACT firstcycle '["'$START_TIME_FORMATTED'", "'$END_TIME'"]' -p $USER@active

