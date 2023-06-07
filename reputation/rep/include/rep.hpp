#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT rep : public contract {
  public:
    using contract::contract;

    ACTION addrepattrib(name org, name badge, name contract, name attribute, uint8_t value);

    ACTION xorgbadges

    ACTION orgbadges(name org, name source_badge, vector<> badges_to_issue)
  private:

    TABLE crossorg {
      name origin_org; // bylaws
      name destination_org; // genesis.eden approved account
      name badge; // meeting particpitaion
      bool origin_org_approved;
      bool destination_org_approved;
      name badge; // participation
      uint8_t count; // 5
    }

    TABLE badgeattr {
      uint64_t id;
      name badge;
      name issued_badge;
      uint64_t count;
    }
    TABLE grouprep {
      uint64_t id;
      uint64_t  group_id;
      name org;
      name badge;
      name attribute;
      uint64_t value;
    };
    typedef multi_index<name("grouprep"), grouprep> grouprep_table;
};
