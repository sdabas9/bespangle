cmake_minimum_required(VERSION 3.10)
project(mrval)
set(MUTUAL_RECOGNITION_INTERFACE_CONTRACT ${MUTUAL_RECOGNITION_INTERFACE_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/mrval.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/mrval.hpp @ONLY)
