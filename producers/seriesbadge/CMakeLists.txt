cmake_minimum_required(VERSION 3.10)
project(seriesbadge)
set(ORCHESTRATOR_CONTRACT_NAME ${ORCHESTRATOR_CONTRACT_NAME})
set(AUTHORITY_CONTRACT ${AUTHORITY_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/seriesbadge.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/seriesbadge.hpp @ONLY)
