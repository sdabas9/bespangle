#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define SIMPLEBADGE_CREATE "nftcircle.gm::createsimple"
#define SIMPLEBADGE_GIVE "nftcircle.gm::givesimple"

CONTRACT edenvalidate : public contract {
  public:
    using contract::contract;

    ACTION addrole(name org, name member, name role) ;
    ACTION membercheck(name org, name contract, name action);
    ACTION memberconstraint (name org, name action, name creator);

    [[eosio::on_notify(SIMPLEBADGE_CREATE)]] void sbcreatcheck (name creator, name badge, vector<name> parentbadge, string ipfsimage, string details);
    [[eosio::on_notify(SIMPLEBADGE_GIVE)]] void sbgivecheck (name from, name to, name badge, string memo);
    
  private:
    TABLE members {
      name    member;
      vector<name>  roles;
      auto primary_key() const { return member.value; }
    };
    typedef multi_index<name("members"), members> members_table;
};
