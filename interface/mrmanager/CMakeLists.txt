cmake_minimum_required(VERSION 3.10)
project(mrmanager)
set(ORG_CHECKS_CONTRACT_NAME ${ORG_CHECKS_CONTRACT})
set(ORCHESTRATOR_CONTRACT ${ORCHESTRATOR_CONTRACT})
set(GOTCHABADGE_CONTRACT ${GOTCHABADGE_CONTRACT})
set(MUTUAL_RECOGNITION_VALIDATION_CONTRACT ${MUTUAL_RECOGNITION_VALIDATION_CONTRACT})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/mrmanager.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/mrmanager.hpp @ONLY)
