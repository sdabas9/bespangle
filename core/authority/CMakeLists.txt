cmake_minimum_required(VERSION 3.10)
project(authority)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/authority.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/authority.hpp @ONLY)
