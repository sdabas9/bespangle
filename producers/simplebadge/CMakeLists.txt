cmake_minimum_required(VERSION 3.10)
project(simplebadge)
set(ORCHESTRATOR_CONTRACT_NAME ${ORCHESTRATOR_CONTRACT_NAME})
set(AUTHORITY_CONTRACT ${AUTHORITY_CONTRACT})
set(BILLING_CONTRACT ${BILLING_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/simplebadge.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/simplebadge.hpp @ONLY)
