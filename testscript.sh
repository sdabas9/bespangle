cleos -u http://jungle4.cryptolions.io push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "testorg11111",
        "to": "billing11111",
        "quantity": "50.0000 EOS",
        "memo": "testorg11111"
      },
      "authorization": [
        {
          "actor": "testorg11111",
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
      "account": "organization",
      "name": "chkscontract",
      "data": {
        "org": "testorg11111",
        "checks_contract": "dummyuser111"
      },
      "authorization": [
        {
          "actor": "testorg11111",
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
      "account": "aabadge11111",
      "name": "initcoll",
      "data": {
        "org": "testorg11111",
        "collection_name": "col1col1col1"
      },
      "authorization": [
        {
          "actor": "testorg11111",
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
      "account": "metadata2222",
      "name": "recognize",
      "data": {
        "org": "testorg11111",
        "trusted_badge_contract": "simplebadge2"
      },
      "authorization": [
        {
          "actor": "testorg11111",
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
      "account": "organization",
      "name": "initsimple",
      "data": {
        "org": "testorg11111",
        "creator": "testorg11111",
        "badge": "b",
        "parent_badges": [],
        "offchain_lookup_data": "{\"img\":\"ipfs\"}",
        "onchain_lookup_data": "{\"name\":\"display name\"}",
        "consumers": [
          "aabadge11111",
          "rollup111111"
        ],
        "memo": ""
      },
      "authorization": [
        {
          "actor": "testorg11111",
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
      "account": "rollup111111",
      "name": "createrollup",
      "data": {
        "org": "testorg11111",
        "emission_name": "t3",
        "activator_criteria": [],
        "emitter_criteria": [
          {
            "key": {
              "issuing_contract": "simplebadge2",
              "asset_name": "a"
            },
            "value": 1
          }
        ],
        "emit_assets": [
          {
            "key": {
              "issuing_contract": "simplebadge2",
              "asset_name": "c"
            },
            "value": 1
          }
        ],
        "cyclic_activator": false,
        "cyclic_emitter": false
      },
      "authorization": [
        {
          "actor": "testorg11111",
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
      "account": "rollup111111",
      "name": "createrollup",
      "data": {
        "org": "testorg11111",
        "emission_name": "compemitter",
        "activator_criteria": [],
        "emitter_criteria": [
          {
            "key": {
              "issuing_contract": "simplebadge2",
              "asset_name": "a"
            },
            "value": 1
          },
          {
            "key": {
              "issuing_contract": "simplebadge2",
              "asset_name": "b"
            },
            "value": 2
          }
        ],
        "emit_assets": [
          {
            "key": {
              "issuing_contract": "simplebadge2",
              "asset_name": "c"
            },
            "value": 1
          }
        ],
        "cyclic_activator": false,
        "cyclic_emitter": false
      },
      "authorization": [
        {
          "actor": "testorg11111",
          "permission": "active"
        }
      ]
    }
  ]
}'
