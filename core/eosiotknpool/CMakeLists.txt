cmake_minimum_required(VERSION 3.10)
project(eosiotknpool)
set(REDEEM_CONTRACT_NAME ${REDEEM_CONTRACT_NAME})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/eosiotknpool.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/eosiotknpool.hpp @ONLY)
