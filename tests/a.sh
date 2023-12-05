cleos -u http://jungle4.cryptolions.io push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "badgetestorg",
        "to": "billingxxxxx",
        "quantity": "1.0000 EOS",
        "memo": "testorgxxxxx"
      },
      "authorization": [
        {
          "actor": "badgetestorg",
          "permission": "active"
        }
      ]
    }
  ]
}'
