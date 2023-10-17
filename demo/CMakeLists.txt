cmake_minimum_required(VERSION 3.16)
project(demo)

set(CMAKE_CXX_STANDARD 20)

#---------------------------------------------------------------
# core

set(CORE ${CMAKE_CURRENT_SOURCE_DIR}/../core)
message(STATUS "core: ${CORE}")

add_subdirectory(${CORE} core)
include_directories(
  ${CORE}/include
  ${CORE}/3rd/spdlog/include
)
message(STATUS "Build with core source code 🆗")

#---------------------------------------------------------------
# app
add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE core)
