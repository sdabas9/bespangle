cmake_minimum_required(VERSION 3.10)
project(org)
set(EDEN_ORG_INTERFACE_CONTRACT ${EDEN_ORG_INTERFACE_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/elections.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/elections.hpp @ONLY)
