# antelopebadges

## Description
This suite of smart contracts allows creation(as simplebadge and gotchabadge) and consume issuance of these badges (as aabadge, cumulative and rounds). See details about individual contracts in the contract folder readme's.

Suite also provides a sample interface contract that an org can deploy. For implementaion of roles and issuance, sample contract used by Eden community is also provided.


## Architecture Diagram
![My Image](architecture_diagram.png)

## Build and deploy

Build and Deploy can be done via build.sh.

### prereqs
- cmake and eosio-cdt are installed. 
- wallet holding the keys for smart contracts should be unlocked.

### steps
Update the following variables in build.sh to build and deploy the smart contract on an antelope based chain.

ORCHESTRATOR_CONTRACT
ACCOUNT_PREFERENCES_CONTRACT
BILLING_CONTRACT
SIMPLEBADGE_CONTRACT
GOTCHABADGE_CONTRACT
AABADGE_CONTRACT
ATOMIC_ASSETS_CONTRACT
ROUNDS_CONTRACT
CUMULATIVE_CONTRACT
CLEOS_URL
