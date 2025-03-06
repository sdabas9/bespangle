cmake_minimum_required(VERSION 3.10)
project(simplebadge)
set(ORCHESTRATOR_CONTRACT ${ORCHESTRATOR_CONTRACT})
set(AUTHORITY_CONTRACT ${AUTHORITY_CONTRACT})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/authorityinterface.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/authorityinterface.hpp @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/simplebadge.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/simplebadge.hpp @ONLY)
