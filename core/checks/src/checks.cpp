#include <checks.hpp>

ACTION checks::schecki(name org, name account, vector<sbadges> own, vector<sbadges> not_own) {
  for(auto i = 0; i < own.size(); i++) {
    own_check(org, account, own[i].badge, own[i].count);
  }
  for(auto i = 0; i < not_own.size(); i++) {
    not_own_check(org, account, not_own[i].badge, not_own[i].count);
  }
}