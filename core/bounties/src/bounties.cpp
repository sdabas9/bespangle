ACTION bounties::cappedbounty(
    name creator,
    name org,
    string description,
    vector<asset> emit_assets,
    extended_asset fungible_asset_per_winner,
    vector<name> reviewers,
    vector<name> participants,
    time_point_sec participation_start_time,
    time_point_sec participation_end_time) {

    require_auth(creator);

    // Invoke remote actions to get the next symbols for badge, emission, and bounty
    action(
        permission_level{get_self(), "active"_n},
        name(ORG_CONTRACT),
        "nextbadge"_n,
        std::make_tuple(org)
    ).send();

    action(
        permission_level{get_self(), "active"_n},
        name(ORG_CONTRACT),
        "nextemission"_n,
        std::make_tuple(org)
    ).send();

    action(
        permission_level{get_self(), "active"_n},
        name(ORG_CONTRACT),
        "nextbounty"_n,
        std::make_tuple(org)
    ).send();

    // Tables in the ORG_CONTRACT
    badgecode_table badgecodes(name(ORG_CONTRACT), org.value);
    emissioncode_table emissioncodes(name(ORG_CONTRACT), org.value);
    bountycode_table bountycodes(name(ORG_CONTRACT), org.value);

    // Retrieve last badge symbol
    auto badge_itr = badgecodes.find(org.value);
    check(badge_itr != badgecodes.end(), "Badge code entry not found for organization");
    symbol badge_symbol = badge_itr->last_badge_symbol;

    // Retrieve last emission symbol
    auto emission_itr = emissioncodes.find(org.value);
    check(emission_itr != emissioncodes.end(), "Emission code entry not found for organization");
    symbol emission_symbol = emission_itr->last_emission_symbol;

    // Retrieve last bounty symbol
    auto bounty_itr = bountycodes.find(org.value);
    check(bounty_itr != bountycodes.end(), "Bounty code entry not found for organization");
    symbol bounty_symbol = bounty_itr->last_bounty_symbol;

    // Update emitter criteria to use the badge symbol
    vector<asset> emitter_criteria = { asset(1, badge_symbol) };

    // Invoke the remote action newemissiona in AND_EMITTER_MANAGER_CONTRACT
    action(
        permission_level{get_self(), "active"_n},
        name(AND_EMITTER_MANAGER_CONTRACT),
        "newemissiona"_n,
        newemissiona_args{
            .authorized = get_self(),
            .emission_symbol = emission_symbol,
            .emitter_criteria = emitter_criteria,
            .emit_badges = emit_assets, // Assuming same assets for emitting badges
            .fungible_asset = fungible_asset_per_winner,
            .cyclic = false
        }
    ).send();

    // Add entry to the bounty table scoped by org
    bounty_table bounties_table(get_self(), org.value);
    bounties_table.emplace(creator, [&](auto& b) {
        b.bounty = bounty_symbol;
        b.emission = emission_symbol;
        b.status = "new"_n;
        b.participation_start_time = participation_start_time;
        b.participation_end_time = participation_end_time;
    });
}
