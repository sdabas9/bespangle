cmake_minimum_required(VERSION 3.10)
project(mrnotify)
set(MUTUAL_RECOGNITION_CONTRACT_NAME ${MUTUAL_RECOGNITION_CONTRACT_NAME})
set(MUTUAL_RECOGNITION_INTERFACE_CONTRACT_NAME ${MUTUAL_RECOGNITION_INTERFACE_CONTRACT_NAME})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/mrnotify.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/mrnotify.hpp @ONLY)
