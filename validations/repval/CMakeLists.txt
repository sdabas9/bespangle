cmake_minimum_required(VERSION 3.10)
project(repval)
set(REPUTATION_INTERFACE_CONTRACT ${REPUTATION_INTERFACE_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/repval.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/repval.hpp @ONLY)
