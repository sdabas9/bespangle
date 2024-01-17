cleos -u http://jungle4.cryptolions.io push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "billingxxxxx",
      "name": "addsettings",
      "data": {
        "key": "maxcredits",
        "value": 10000
      },
      "authorization": [
        {
          "actor": "billingxxxxx",
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
      "account": "billingxxxxx",
      "name": "addsettings",
      "data": {
        "key": "ramrate",
        "value": 6000
      },
      "authorization": [
        {
          "actor": "billingxxxxx",
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
      "account": "billingxxxxx",
      "name": "addsettings",
      "data": {
        "key": "bytespercr",
        "value": 100
      },
      "authorization": [
        {
          "actor": "billingxxxxx",
          "permission": "active"
        }
      ]
    }
  ]
}'
