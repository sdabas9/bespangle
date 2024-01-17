cmake_minimum_required(VERSION 3.10)
project(simpleval)
set(SIMPLE_INTERFACE_CONTRACT ${SIMPLE_INTERFACE_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/simpleval.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/simpleval.hpp @ONLY)
