#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <string>

using namespace eosio;
using namespace std;

#define ORG_CONTRACT "organizayyyy"

TABLE orgs {
    name org;         // Organization identifier, used as primary key
    name org_code;    // Converted org_code, ensuring uniqueness and specific format
    name checks_contract;
    string offchain_lookup_data;
    string onchain_lookup_data;

    // Specify the primary key
    auto primary_key() const { return org.value; }

    // Specify a secondary index for org_code to ensure its uniqueness
    uint64_t by_org_code() const { return org_code.value; }
};

// Declare the table
typedef eosio::multi_index<"orgs"_n, orgs,
    eosio::indexed_by<"orgsidx"_n, eosio::const_mem_fun<orgs, uint64_t, &orgs::by_org_code>>
> orgs_index;

// Function to retrieve organization from symbol
name get_org_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
    string _symbol_str = _symbol.code().to_string(); // Convert symbol to string
    check(_symbol_str.size() >= 4, failure_identifier + "symbol must have at least 4 characters.");

    // Extract the first 4 characters as org_code
    string org_code_str = _symbol_str.substr(0, 4);

    for (auto & c: org_code_str) {
        c = tolower(c);
    }
    name org_code = name(org_code_str);

    // Set up the orgcode table and find the org_code
    orgs_index _orgs(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
    auto org_code_itr = _orgs.get_index<"orgsidx"_n>().find(org_code.value);

    check(org_code_itr != _orgs.get_index<"orgsidx"_n>().end(), failure_identifier + "Organization code not found.");
    check(org_code_itr->org_code == org_code, failure_identifier + "Organization code not found.");
    
    // Assuming the org is stored in the same row as the org_code
    return org_code_itr->org; // Return the found organization identifier
}

name get_name_from_internal_symbol(const symbol& _symbol, string failure_identifier) {
    string _symbol_str = _symbol.code().to_string(); // Convert symbol to string
    check(_symbol_str.size() == 7, failure_identifier + "Symbol must have at least 7 characters.");

    // Extract the first 4 characters as org_code
    string _str = _symbol_str.substr(4, 7);

    for (auto & c: _str) {
        c = tolower(c);
    }
    return name(_str);
}

void notify_checks_contract(name org) {
    orgs_index _orgs( name(ORG_CONTRACT), name(ORG_CONTRACT).value );
    auto itr = _orgs.find(org.value);
    if(itr != _orgs.end()) {
    require_recipient(itr->checks_contract);
    }
}



