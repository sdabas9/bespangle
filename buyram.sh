cleos -u http://jungle4.cryptolions.io push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio",
      "name": "buyram",
      "data": {
        "payer": "interface111",
        "receiver": "interface111",
        "quant": "150.0000 EOS"
      },
      "authorization": [
        {
          "actor": "interface111",
          "permission": "active"
        }
      ]
    }
  ]
}'


cleos -u http://jungle4.cryptolions.io push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio",
      "name": "buyram",
      "data": {
        "payer": "router111111",
        "receiver": "router111111",
        "quant": "10.0000 EOS"
      },
      "authorization": [
        {
          "actor": "router111111",
          "permission": "active"
        }
      ]
    }
  ]
}'

cleos -u http://jungle4.cryptolions.io push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio",
      "name": "buyram",
      "data": {
        "payer": "basicissue11",
        "receiver": "basicissue11",
        "quant": "90.0000 EOS"
      },
      "authorization": [
        {
          "actor": "basicissue11",
          "permission": "active"
        }
      ]
    }
  ]
}'


cleos -u http://jungle4.cryptolions.io push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio",
      "name": "buyram",
      "data": {
        "payer": "accounting11",
        "receiver": "accounting11",
        "quant": "99.0000 EOS"
      },
      "authorization": [
        {
          "actor": "accounting11",
          "permission": "active"
        }
      ]
    }
  ]
}'


cleos -u http://jungle4.cryptolions.io push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio",
      "name": "buyram",
      "data": {
        "payer": "andemitter11",
        "receiver": "andemitter11",
        "quant": "150.0000 EOS"
      },
      "authorization": [
        {
          "actor": "andemitter11",
          "permission": "active"
        }
      ]
    }
  ]
}'




