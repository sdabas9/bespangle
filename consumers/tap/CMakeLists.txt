cmake_minimum_required(VERSION 3.10)
project(tap)
set(ORCHESTRATOR_CONTRACT_NAME ${ORCHESTRATOR_CONTRACT_NAME})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/tap.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/tap.hpp @ONLY)
