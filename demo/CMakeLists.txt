cmake_minimum_required(VERSION 3.16)
project(demo)

set (CMAKE_CXX_FLAGS "-std=c++17 -Wall -DLLVM_USE_LINKER=lld -Wno-missing-braces -Wno-unused-local-typedef")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")

#---------------------------------------------------------------
# core

set(CORE ${CMAKE_CURRENT_SOURCE_DIR}/../core)
message(STATUS "core: ${CORE}")
add_subdirectory(${CORE} core)
message(STATUS "Build with core source code 🆗")

#---------------------------------------------------------------
# app
file(GLOB SOURCES "src/*.cpp" "main.cpp")
add_executable(${PROJECT_NAME}
  ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE core)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CORE}/include
  ${CORE}/3rd/spdlog/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)