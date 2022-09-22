# org smart contract

This smart contract allows an org to produce and to consume the produced badges.


## Producers and Consumers

Early dive into these two broad categories makes for easier understanding.

### Producer Badge Contracts
Producers Badge Contracts produce the badges. Two Producer Contracts are available -
1) Simple Badge Contract - Creates a badge, which a person of authority in an org can issue to  members of org.
2) Gotcha Badge Contact - Creates a badge which members of org can issue to other members. A gotcha badge comes with Supply Cap for a Time Duration Cycle. Supply Cap is restored at end of every Time Duration Cycle.

### Consumer Badge Contracts

Three Consumers Contracts are available -

1) Atomic Assets Consumer - This consumes badges at granular level in atomic asset standard. i.e. for every Badge a non transferrable asset is created in atomic assets standard. 
2) Cumulative Consumer - This consumers badges at cumulative level. Same badges are added and stored as one record.
3) Rounds Consumer - This consumer accumulate badges and their corresponding scores from the start to the end of the round. Start and End of round are triggered by calling actions.

## Usage

For org to use above features, it need to -
### 1 -  Deploy org smart contract and validation contract.
Deploy this contract and a validation contract similar to Eden Validation Contract.

### 2 - Initialize org contract

Org would need to do some basic setup, which involves
-Subscribing to the producer contracts.
-Subscribing to a validation contract. Org needs to implement this to validate the input of actions. See Eden Validation Contract.
-If an org would want to subscribe to Atomic Asset as consumer for the badges, it would need to call this action.

All this can be done via this action -
```    
ACTION initsystem(name checks_contract, 
      vector<name> badges_types, 
      name aacollection);

      checks_contract - org authored contract to validate input fields of various actions. Mainly this can be used to check authority of action execution.
      badges_types - vector of producer contracts like simple badge contract and gotcha badge contract
      aacollection - atomic asset collection name
```

### 3 - Badge definition 
With initial setup out of the way, next an org can define some simple badges and gotcha badge.
```
    ACTION initsimple (name creator, 
      name badge, 
      string ipfs_image, 
      string display_name, 
      vector<name> features);
```


```
    ACTION initgotcha (name creator, 
      name badge, 
      time_point_sec starttime, 
      uint64_t cycle_length, 
      uint8_t supply_per_cycle, 
      string ipfs_image, 
      string display_name, 
      vector<name> features);
```

### 4 - Badge Issuance
With badge creation out of the way, members and issuing authority can start issue badges via actions.
```
ACTION givesimple (name badge, name authorizer, name to, string memo );
```

```
ACTION givegotcha (name badge, name from, name to, uint8_t amount, string memo );
```
### 5 - Other details about rounds consumer
Addtional info about rounds consumer contract - 

To utilize features of Rounds Consumer, org needs to create a round, add badges and their corresponding scores and start the round. A round can be ended by calling endround action.

Account that receives the badges, need to opt in to receive badges from an org. This can be done via action -

