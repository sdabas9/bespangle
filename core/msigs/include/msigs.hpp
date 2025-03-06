#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <vector>
#include <eosio/permission.hpp>
#include <algorithm>


using namespace eosio;

class [[eosio::contract]] msigs : public contract {
public:
    using contract::contract;

   ACTION consumesimple(
      name originating_contract;
      name originating_contract_key;
      name requester, 
      vector<name> approvers, 
      name to, symbol badge_symbol, string memo,
      string request_memo);                  
   ACTION approve(name approver, name org, uint64_t request_id, string stream_reason);
   ACTION reject(name approver, name org, uint64_t request_id, string stream_reason);
   //ACTION rfe(name approver, name org, uint64_t request_id, string stream_reason);
   ACTION evidence(name requester, name org, uint64_t request_id, string stream_reason);
   ACTION withdraw(name requester, name org, uint64_t request_id, string stream_reason);
   ACTION execute(name requester, name org, uint16_t batch_size);
   ACTION cleanup(name org);
private:

    TABLE request {
        uint64_t request_id;
        name action;
        name requester;
        name originating_contract;
        name originating_contract_key;
        vector<pair<name, name>> approvers;
        name status;
        time_point_sec expiration_time;

        auto primary_key() const { return request_id; }
        uint64_t by_status() const { return status.value; }
    };
    typedef multi_index<"requests"_n, request,
        indexed_by<"bystatus"_n, const_mem_fun<request, uint64_t, &request::by_status>>
    > request_index;

    TABLE simissue {
        uint64_t request_id;
        name to;
        symbol badge_symbol;
        string memo;

        auto primary_key() const { return request_id; }
    };
    typedef multi_index<"simissues"_n, simissue> simissue_index;

    TABLE sequence {
        uint64_t next_request_id;

        auto primary_key() const { return next_request_id; }
    };
    typedef multi_index<"sequences"_n, sequence> sequence_index;


    TABLE orgcode {
      name org;         // Organization identifier, used as primary key
      name org_code;    // Converted org_code, ensuring uniqueness and specific format

      // Specify the primary key
      auto primary_key() const { return org.value; }

      // Specify a secondary index for org_code to ensure its uniqueness
      uint64_t by_org_code() const { return org_code.value; }
    };

    // Declare the table
    typedef eosio::multi_index<"orgcodes"_n, orgcode,
      eosio::indexed_by<"orgcodeidx"_n, eosio::const_mem_fun<orgcode, uint64_t, &orgcode::by_org_code>>
    > orgcode_index;


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
      orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
      auto org_code_itr = orgcodes.get_index<"orgcodeidx"_n>().find(org_code.value);

      check(org_code_itr != orgcodes.get_index<"orgcodeidx"_n>().end(), failure_identifier + "Organization code not found.");
      check(org_code_itr->org_code == org_code, failure_identifier + "Organization code not found.");
      // Assuming the org is stored in the same row as the org_code
      return org_code_itr->org; // Return the found organization identifier
    }
};
