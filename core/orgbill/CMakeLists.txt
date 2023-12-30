cmake_minimum_required(VERSION 3.10)
project(orgbill)
set(AUTHORITY_CONTRACT ${AUTHORITY_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/orgbill.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/orgbill.hpp @ONLY)
