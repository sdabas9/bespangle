cmake_minimum_required(VERSION 3.10)
project(orchmanager)
set(ORG_CONTRACT ${ORG_CONTRACT})
set(ORCHESTRATOR_CONTRACT ${ORCHESTRATOR_CONTRACT})
set(ORCHESTRATOR_VALIDATION_CONTRACT ${ORCHESTRATOR_VALIDATION_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/orchmanager.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/orchmanager.hpp @ONLY)
