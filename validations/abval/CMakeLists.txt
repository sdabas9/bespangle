cmake_minimum_required(VERSION 3.10)
project(abval)
set(ANTIBADGE_INTERFACE_CONTRACT ${ANTIBADGE_INTERFACE_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/abval.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/abval.hpp @ONLY)
