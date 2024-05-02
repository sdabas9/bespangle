#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define BOUNDED_AGG_CONTRACT "baggyyyyyyyy"
#define ORCHESTRATOR_CONTRACT "orchyyyyyyyy"
#define ORG_CONTRACT "organizayyyy"
#define BOUNDED_STATS_CONTRACT "bstatsyyyyyy"
#define BOUNDED_AGG_VALIDATION_CONTRACT "boundaggvaly"

CONTRACT bamanager : public contract {
  public:
    using contract::contract;

    ACTION initagg(name authorized, symbol agg_symbol, vector<symbol> badge_symbols, vector<symbol> stats_badge_symbols, string agg_description);
    ACTION addinitbadge(name authorized, symbol agg_symbol, vector<symbol> badge_symbols);
    ACTION reminitbadge(name authorized, symbol agg_symbol, vector<symbol> badge_symbols);
    ACTION addstatbadge(name authorized, symbol agg_symbol, vector<symbol> badge_symbols);
    ACTION remstatbadge(name authorized, symbol agg_symbol, vector<symbol> badge_symbols);


    ACTION initseq(name authorized, symbol agg_symbol, string sequence_description);
    ACTION actseq(name authorized, symbol agg_symbol, vector<uint64_t> seq_ids);


    ACTION actseqai(name authorized, symbol agg_symbol);
    ACTION actseqfi(name authorized, symbol agg_symbol);
    ACTION endseq(name authorized, symbol agg_symbol, vector<uint64_t> seq_ids);
    ACTION endseqaa(name authorized, symbol agg_symbol);
    ACTION endseqfa(name authorized, symbol agg_symbol);
    ACTION addbadge(name authorized, symbol agg_symbol, vector<uint64_t> seq_ids, vector<symbol> badge_symbols);

    ACTION pauseall(name authorized, symbol agg_symbol, uint64_t seq_id);
    ACTION pausebadge(name authorized, symbol agg_symbol, uint64_t badge_agg_seq_id);
    ACTION pausebadges(name authorized, symbol agg_symbol, uint64_t seq_id, vector<symbol> badge_symbols);
    ACTION pauseallfa(name authorized, symbol agg_symbol);
    ACTION resumeall(name authorized, symbol agg_symbol, uint64_t seq_id);
    ACTION resumebadge(name authorized, symbol agg_symbol, uint64_t badge_agg_seq_id);
    ACTION resumebadges(name authorized, symbol agg_symbol, uint64_t seq_id, vector<symbol> badge_symbols);

  private:

  
    TABLE checks {
      name org;
      name checks_contract;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("checks"), checks> checks_table;

    void notify_checks_contract(name org) {
      checks_table _checks( name(ORG_CONTRACT), name(ORG_CONTRACT).value );
      auto itr = _checks.find(org.value);
      if(itr != _checks.end()) {
        require_recipient(itr->checks_contract);
      }
    }
    

        // Define the structure of the table
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

    string get_org_code(name org, string failure_identifier) {
      orgcode_index orgcodes(name(ORG_CONTRACT), name(ORG_CONTRACT).value);
      auto iterator = orgcodes.find(org.value);
      check(iterator != orgcodes.end(), failure_identifier + "Organization not found");
      return iterator->org_code.to_string();
    }

    void validate_badge(name badge, string failure_identifier) {
      string badge_str = badge.to_string();
      check(badge_str.length() == 3, failure_identifier + "badge code must be exactly 3 characters long");
      for (char c : badge_str) {
        check(std::isalpha(c), failure_identifier + "badge code can only contain alphabets");
      }
    }

    void validate_agg(name agg, string failure_identifier) {
      string agg_str = agg.to_string();
      check(agg_str.length() == 3, failure_identifier + "agg code must be exactly 3 characters long");
      for (char c : agg_str) {
        check(std::isalpha(c), failure_identifier + "agg code can only contain alphabets");
      }
    }

    symbol validate_and_get_symbol(name org, name badge, string failure_identifier) {
      string org_code = get_org_code(org, failure_identifier);
      validate_badge(badge, failure_identifier);
      string symbol_string = org_code + badge.to_string();
      std::transform(symbol_string.begin(), symbol_string.end(), symbol_string.begin(), ::toupper);
      return symbol(symbol_code(symbol_string), 0);
    }

    symbol validate_and_get_agg_symbol(name org, name agg, string failure_identifier) {
      string org_code = get_org_code(org, failure_identifier);
      validate_agg(agg, failure_identifier);
      string symbol_string = org_code + agg.to_string();
      std::transform(symbol_string.begin(), symbol_string.end(), symbol_string.begin(), ::toupper);
      return symbol(symbol_code(symbol_string), 0);
    }

    name get_org_from_badge_symbol(const symbol& badge_symbol, string failure_identifier) {
        string badge_symbol_str = badge_symbol.code().to_string(); // Convert symbol to string
        check(badge_symbol_str.size() >= 4, failure_identifier + "Badge symbol must have at least 4 characters.");

        // Extract the first 4 characters as org_code
        string org_code_str = badge_symbol_str.substr(0, 4);

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

    name get_org_from_agg_symbol(const symbol& agg_symbol, string failure_identifier) {
        string agg_symbol_str = agg_symbol.code().to_string(); // Convert symbol to string
        check(agg_symbol_str.size() >= 4, failure_identifier + "Aggregation symbol must have at least 4 characters.");

        // Extract the first 4 characters as org_code
        string org_code_str = agg_symbol_str.substr(0, 4);

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


    struct initagg_args {
      name org;
      symbol agg_symbol;
      vector<symbol> init_badge_symbols;
      string agg_description;
    };

    struct stats_activate_args {
      name org;
      symbol agg_symbol;
      vector<symbol> badge_symbols;
    };

    struct stats_deactivate_args {
      name org;
      symbol agg_symbol;
      vector<symbol> badge_symbols;
    };

    struct addinitbadge_args {
      name org;
      symbol agg_symbol;
      vector<symbol> badge_symbols;
    };

    struct reminitbadge_args {
      name org;
      symbol agg_symbol;
      vector<symbol> badge_symbols;
    };


    struct initseq_args {
      name org;
      symbol agg_symbol;
      string sequence_description;
    };

    struct actseq_args {
      name org;
      symbol agg_symbol;
      vector<uint64_t> seq_ids;
    };

    struct actseqai_args {
      name org;
      symbol agg_symbol;
    };

    struct actseqfi_args {
      name org;
      symbol agg_symbol;
    };

    struct endseq_args {
      name org;
      symbol agg_symbol;
      vector<uint64_t> seq_ids;
    };
 
    struct endseqaa_args {
      name org;
      symbol agg_symbol;
    };

    struct endseqfa_args {
      name org;
      symbol agg_symbol;
    };

    struct addbadge_args {
      name org;
      symbol agg_symbol;
      vector<uint64_t> seq_ids;
      vector<symbol> badge_symbols;
    };

    struct addbadgefa_args {
      name org;
      symbol agg_symbol;
      vector<symbol> badge_symbols;
    };

    struct addbadgeaa_args {
      name org;
      symbol agg_symbol;
      vector<symbol> badge_symbols;
    };

    struct addbadgefi_args {
      name org;
      symbol agg_symbol;
      vector<symbol> badge_symbols;
    };  

    struct addbadgeai_args {
      name org;
      symbol agg_symbol;
      vector<symbol> badge_symbols;
    };

    struct addfeature_args {
      name org;
      symbol badge_symbol;
      name notify_account;
      string memo;
    };

    struct pauseall_args {
      name org;
      symbol agg_symbol;
      uint64_t seq_id;
    };

    struct pausebadge_args {
      name org;
      symbol agg_symbol;
      uint64_t badge_agg_seq_id;
    };

    struct pausebadges_args {
      name org;
      symbol agg_symbol;
      uint64_t seq_id;
      vector<symbol> badge_symbols;
    };

    struct pauseallfa_args {
      name org;
      symbol agg_symbol;
    };

    struct resumeall_args {
      name org;
      symbol agg_symbol;
      uint64_t seq_id;
    };

    struct resumebadge_args {
      name org;
      symbol agg_symbol;
      uint64_t badge_agg_seq_id;
    };

    struct resumebadges_args {
      name org;
      symbol agg_symbol;
      uint64_t seq_id;
      vector<symbol> badge_symbols;
    };

};


