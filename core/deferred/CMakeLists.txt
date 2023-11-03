cmake_minimum_required(VERSION 3.10)
project(deferred)
set(NOTIFICATION_CONTRACT_NAME ${NOTIFICATION_CONTRACT_NAME})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/deferred.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/deferred.hpp @ONLY)
