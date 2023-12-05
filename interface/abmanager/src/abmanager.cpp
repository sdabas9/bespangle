#include <abmanager.hpp>

ACTION abmanager::create(name org, name authorized,
              name antibadge,
              name badge,
              name type,
              string offchain_lookup_data,
              string onchain_lookup_data,
              vector<name> consumers,
              string memo) {

  require_auth(authorized);

  notify_checks_contract(org);
  require_recipient(name(ANTIBADGE_VALIDATION_CONTRACT));

  action(
    permission_level{get_self(), name("active")},
    get_self(),
    name("ncreate"),
    create_args {
      .org = org,
      .antibadge = antibadge,
      .badge = badge,
      .type = type,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo
    }
  ).send();

  for (auto i = 0; i < consumers.size(); i++) {
    action(
      permission_level{get_self(), name("active")},
      get_self(),
      name("naddfeatur"),
      addfeature_args {
        .org = org,
        .badge_name = antibadge,
        .notify_account = consumers[i],
        .memo = memo
      }
    ).send();
  }    
}

ACTION abmanager::createinv(name org, name authorized,
                 name antibadge,
                 name badge,
                 string offchain_lookup_data,
                 string onchain_lookup_data,
                 string memo) {

  require_auth(authorized);

  notify_checks_contract(org);
  require_recipient(name(ANTIBADGE_VALIDATION_CONTRACT));

  action(
    permission_level{get_self(), name("active")},
    get_self(),
    name("ncreateinv"),
    createinv_args {
      .org = org,
      .antibadge = antibadge,
      .badge = badge,
      .offchain_lookup_data = offchain_lookup_data,
      .onchain_lookup_data = onchain_lookup_data,
      .memo = memo
    }
  ).send();
}

ACTION abmanager::issue(name org, name authorized,
              name to,
              name antibadge,
              uint64_t amount,
              string memo) {

  require_auth(authorized);

  notify_checks_contract(org);
  require_recipient(name(ANTIBADGE_VALIDATION_CONTRACT));

  action(
    permission_level{get_self(), name("active")},
    get_self(),
    name("nissue"),
    issue_args {
      .org = org,
      .to = to,
      .antibadge = antibadge,
      .amount = amount,
      .memo = memo
    }
  ).send();
}

ACTION abmanager::naddfeatur (name org,
  name badge_name, 
  name notify_account, 
  string memo) {
  require_auth(get_self());
  require_recipient(name(NOTIFICATION_CONTRACT));
}

ACTION abmanager::ncreate(name org,
                name antibadge,
                name badge,
                name type,
                string offchain_lookup_data,
                string onchain_lookup_data,
                string memo) {
  require_auth(get_self());
  require_recipient(name(ANTIBADGE_NOTIFICATION_CONTRACT));  
}

ACTION abmanager::ncreateinv(name org,
                  name antibadge,
                  name badge,
                  string offchain_lookup_data,
                  string onchain_lookup_data,
                  string memo) {
  require_auth(get_self());
  require_recipient(name(ANTIBADGE_NOTIFICATION_CONTRACT));
}

ACTION abmanager::nissue(name org,
              name to,
              name antibadge,
              uint64_t amount,
              string memo) {
  require_auth(get_self());
  require_recipient(name(ANTIBADGE_NOTIFICATION_CONTRACT));
}
