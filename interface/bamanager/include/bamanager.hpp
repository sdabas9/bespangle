#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

#define BOUNDED_AGG_CONTRACT "baggzzzzzzzz"
#define ORCHESTRATOR_CONTRACT "orchzzzzzzzz"
#define ORG_CONTRACT "orgzzzzzzzzz"
#define BOUNDED_AGG_VALIDATION_CONTRACT "boundaggvalz"

CONTRACT bamanager : public contract {
  public:
    using contract::contract;

    ACTION initagg(name authorized, name org, name agg, string agg_description);
    ACTION initseq(name authorized, name org, name agg, string sequence_description);
    ACTION actseq(name authorized, name org, name agg, vector<uint64_t> seq_ids, vector<name> badges);
    ACTION actseqai(name authorized, name org, name agg, vector<name> badges);
    ACTION actseqfi(name authorized, name org, name agg, vector<name> badges);
    ACTION endseq(name authorized, name org, name agg, vector<uint64_t> seq_ids);
    ACTION endseqaa(name authorized, name org, name agg);
    ACTION endseqfa(name authorized, name org, name agg);
    ACTION addbadge(name authorized, name org, name agg, vector<uint64_t> seq_ids, vector<name> badges);

    ACTION pauseall(name authorized, name org, name agg, uint64_t seq_id);
    ACTION pausebadge(name authorized, name org, name agg, uint64_t badge_agg_seq_id);
    ACTION pausebadges(name authorized, name org, name agg, uint64_t seq_id, vector<name> badges);
    ACTION pauseallfa(name authorized, name org, name agg);
    ACTION resumeall(name authorized, name org, name agg, uint64_t seq_id);
    ACTION resumebadge(name authorized, name org, name agg, uint64_t badge_agg_seq_id);
    ACTION resumebadges(name authorized, name org, name agg, uint64_t seq_id, vector<name> badges);

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


    struct initagg_args {
      name org;
      symbol agg_symbol;
      string agg_description;
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

