# rounds


## description
This consumer contract has actions to create rounds and change status of those rounds to start, end, pause and resume.
This contract also has an action to add badges to a round and how that badge will contribute to account score in that round.
As the new badges are received by an acccount, score is updated for an account. see examples at end of readme.

## actions

### create a round with init status
```
ACTION createround (name org, 
    name round, 
    string description, 
    bool account_constrained, 
    vector<name> participating_accounts); 
 ```

### start a round
```
ACTION activeround (name org, name round);
```

### end a round
```
ACTION endround (name org, name round);
```

### pause a round
```
ACTION pauseround (name org, name round);
```

### resume a round
```
ACTION resumeround (name org, name round);
```

### add badge to a round
```
ACTION addscoremeta (name org,
    name round,
    uint64_t badge_id,
    name balance_based_scoring_type,
    uint16_t balance_based_scoring_weight,
    name source_based_scoring_type,
    uint16_t source_based_scoring_weight);
```

**balance_based_scoring_type** - linear OR fibonacci OR empty <br />
**balance_based_scoring_weight** - weight of the balance_based_scoring_type <br />
**source_based_scoring_type** - linear OR fibonacci OR empty. <br />
**source_based_scoring_weight** - weight of the source_based_scoring_type <br />
#### EXAMPLES
##### eg 1 scenario
Badge - b1 <br />
balance_based_scoring_type - linear <br />
balance_based_scoring_weight - 10 <br />
source_based_scoring_type - linear <br />
source_based_scoring_weight - 100<br />
<br />
An account receives 5 b1 from 3 accounts. <br />
##### eg 1 score
Its balance based score would be (number of badges recieved * balance_based_scoring_weight)= 5 * 10 = 50<br />
Its source based score would be ( number of accounts from which badge is received * source_based_scoring_weight ) = 3 * 100 <br />
Total score for b1  ( balance based score + source based score ) = 50 + 300 <br />
<br />
##### eg 2 scenario
Badge - b1 <br />
balance_based_scoring_type - linear <br />
balance_based_scoring_weight - 10 <br />
source_based_scoring_type - fibonacci <br />
source_based_scoring_weight - 100 <br />
<br />
An account receives 5 b1 from 3 accounts. <br />
##### eg 2 score
Its balance based score would be (number of badges recieved * balance_based_scoring_weight)= 5 * 10 = 50 <br />
Its source based score would be (fib(1) + fib(2) + fib(3)) * source_based_scoring_weight  = (1+1+2) * 100 <br />
Total score for b1  ( balance based score + source based score ) = 50 + 400 <br />



