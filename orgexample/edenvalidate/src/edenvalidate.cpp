#include <edenvalidate.hpp>


ACTION edenvalidate::addmember (name member) {
  require_auth(get_self());
  members_table _members (get_self(), get_self().value);
  auto itr = _members.find(member.value);
  check (itr == _members.end(), "<member> already exists") ;
  vector<name> roles ;
  roles.push_back(name("member"));
  _members.emplace (get_self(), [&]( auto& row) {
      row.member = member;
      row.roles = roles;
  });
}

ACTION edenvalidate::addrole(name member, name role) {
  // admin, issuer, creater, member
  require_auth(get_self());

  members_table _members (get_self(), get_self().value);
  auto itr = _members.find(member.value);
  check (itr != _members.end(), "<member> not found") ;

  vector<name> roles = itr->roles;

  for(auto i = 0 ; i < roles.size(); i++) {
    check (role != roles[i], "<role> already exists");
  }

  roles.push_back(role);

  _members.modify( itr, get_self(), [&]( auto& row ) {
    row.roles = roles;
  });

}

void edenvalidate::simcreatechk (name creator, 
  name badge, 
  vector<name> parent_badges,
  string offchain_lookup_data, 
  string onchain_lookup_data,
  vector<name> consumers,
  string memo) {
  check(hasRole(creator, name("simcreator")), "<creator> does not have permission to create a simple badge");
  //check(hasBadgeRestriction(badge, creator, "")
}

void edenvalidate::simgivechk (name badge, name authorizer, name to, string memo ) {
  check(hasRole(authorizer, name("simgiver")), "<authorizer> does not have permission to give a simple badge");
}

void edenvalidate::serdefchk (name creator, name family) {
  check(hasRole(creator, name("sedecreator")), "creator does not have permission to define series badge");
}

void edenvalidate::sercreatechk (name creator, 
  name family, 
  name badge, 
  string offchain_lookup_data, 
  string onchain_lookup_data,
  vector<name> consumers,
  string memo) {
  check(hasRole(creator, name("sebacreator")), "creator does not have permission to create next series badge");
}

void edenvalidate::sergivechk (name issuer, name family, name to, string memo) {
  check(hasRole(issuer, name("sebagiver")), "issuer does not have permission to issue next series badge");
}
