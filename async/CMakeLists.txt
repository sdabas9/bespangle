cmake_minimum_required(VERSION 3.10)
project(async)
set(ORG_INTERFACE_CONTRACT ${ORG_INTERFACT_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/async.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/async.hpp @ONLY)
