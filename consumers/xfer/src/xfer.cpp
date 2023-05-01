#include <xfer.hpp>

ACTION xfer::transferrable(name org, name assetname) {

}


ACTION xfer::transfer(name org, name assetname, name from, name to,uint64_t amount, string memo) {
  require_auth(from);

}



EOSIO_DISPATCH(xfer, (hi)(clear))
