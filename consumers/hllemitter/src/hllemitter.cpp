#include <hllemitter.hpp>

void hllemitter::notifyachiev(
    name org, 
    name badge,
    name account, 
    name from,
    uint64_t count,
    string memo,  
    vector<name> notify_accounts
) {

    // Create an instance of the emissions_table scoped by the organization
    emissions_table emissions(_self, org.value);

    // Check if an entry with the specified badge  exists
    auto existing_emission = emissions.find(badge.value);
    name emit_badge ;

    check(existing_emission != emissions.end(), "Uniqueness badge not defined for a badge with hll consumer");
    emit_badge = existing_emission->sender_uniqueness_badge;
    
    // Create an instance of the balances_table scoped by the organization
    balances_table balances(_self, org.value);

    // Generate composite key: combination of account and badge
    uint128_t composite_key = (static_cast<uint128_t>(account.value) << 64) | badge.value;

    // Find an existing record by composite key
    auto secondary_index = balances.get_index<"accountbadge"_n>();
    auto existing_entry = secondary_index.find(composite_key);
    uint8_t b = 7;
    uint32_t m = 1 << b;
    if (existing_entry == secondary_index.end() || 
        existing_entry->account != account ||
        existing_entry->badge != badge) {
        // Initialize HyperLogLog data structure
        vector<uint8_t> M(m, 0);
        hll::HyperLogLog hll(b, m, M);
        hll.add(from.to_string().c_str(), from.to_string().size());

        // No existing entry found, insert a new record
        balances.emplace(_self, [&](auto& row) {
            row.id = balances.available_primary_key(); // Generate next available primary key
            row.account = account;
            row.badge = badge;
            row.hll = hll.registers();
        });
        action {
            permission_level{get_self(), name("active")},
            name(SIMPLEBADGE_CONTRACT),
            name("issue"),
            issue_args {
            .org = org,
            .to = account,
            .badge = emit_badge,
            .amount = 1,
            .memo = "issued from rollup consumer"}
        }.send(); 
    } else if (existing_entry != secondary_index.end() &&
         existing_entry->account == account && 
         existing_entry->badge == badge) {
        bool increment = false;
        
        // Get existing HyperLogLog data
        vector<uint8_t> M = existing_entry->hll;
        hll::HyperLogLog hll(b, m, M);
        
        // Estimate cardinality before and after adding new element
        double estimate_before = hll.estimate();
        hll.add(from.to_string().c_str(), from.to_string().size());
        double estimate_after = hll.estimate();

        // Check if cardinality has increased
        if (estimate_before != estimate_after) {
            secondary_index.modify(existing_entry, get_self(), [&](auto& row) {
                row.hll = hll.registers();
            });
            action {
                permission_level{get_self(), name("active")},
                name(SIMPLEBADGE_CONTRACT),
                name("issue"),
                issue_args {
                .org = org,
                .to = account,
                .badge = emit_badge,
                .amount = 1,
                .memo = "issued from rollup consumer"}
            }.send(); 
        }
    }
}


ACTION hllemitter::newemission(
    name org,
    name badge,
    name sender_uniqueness_badge
) {
    check_internal_auth(name("newemission"));
    check(badge != sender_uniqueness_badge, "badge and sender_uniqueness_badge can not be same");
    // Create an instance of the emissions_table scoped by the organization
    emissions_table emissions(_self, org.value);

    // Check if an entry with the specified emission_name already exists
    auto existing_emission = emissions.find(badge.value);

    // Ensure that the emission does not already exist
    eosio::check(existing_emission == emissions.end(), "Emission with this name already exists");

    // Insert the new emission into the table with status set to "INIT"
    emissions.emplace(_self, [&](auto& new_emission) {
        new_emission.badge = badge;
        new_emission.sender_uniqueness_badge = sender_uniqueness_badge;
    });
}








