cmake_minimum_required(VERSION 3.10)
project(hllval)
set(ORG_CONTRACT ${ORG_CONTRACT})
set(HLL_EMITTER_MANAGER_CONTRACT ${HLL_EMITTER_MANAGER_CONTRACT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/hllval.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/hllval.hpp @ONLY)
