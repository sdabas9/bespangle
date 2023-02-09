#include <checks.hpp>

ACTION checks::schecki(name org, name account, vector<sbadges> own, vector<sbadges> does_not_own) {
  for(auto i = 0; i < own.size(); i++) {
    own_check(org, account, name(SIMPLEBADGE_CONTRACT_NAME), own[i].badge, own[i].count);
  }
  for(auto i = 0; i < does_not_own.size(); i++) {
    does_not_own_check(org, account, name(SIMPLEBADGE_CONTRACT_NAME), does_not_own[i].badge, does_not_own[i].count);
  }
}