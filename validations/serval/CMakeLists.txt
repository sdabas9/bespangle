cmake_minimum_required(VERSION 3.10)
project(serval)
set(SERIES_INTERFACE_CONTRACT ${SERIES_INTERFACE_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/serval.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/serval.hpp @ONLY)
