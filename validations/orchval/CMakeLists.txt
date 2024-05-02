cmake_minimum_required(VERSION 3.10)
project(orchval)
set(ORCHESTRATOR_MANAGER_CONTRACT ${ORCHESTRATOR_MANAGER_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/orchval.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/orchval.hpp @ONLY)
