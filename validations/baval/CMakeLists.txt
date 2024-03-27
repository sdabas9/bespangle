cmake_minimum_required(VERSION 3.10)
project(baval)
set(BOUNDED_AGG_MANAGER_CONTRACT ${BOUNDED_AGG_MANAGER_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/baval.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/baval.hpp @ONLY)
