#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <string>

using namespace eosio;
using namespace std;

#define ANDEMITTER_CONTRACT "andemitteryy"

// Autocode table definition (for tracking last_auto_symbol)
TABLE autocode {
    name    org;             // Organization
    symbol  last_auto_symbol; 

    uint64_t primary_key() const { return org.value; }
};
typedef multi_index<"autocode"_n, autocode> autocode_table;
