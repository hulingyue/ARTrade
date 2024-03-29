cmake_minimum_required(VERSION 3.16)
project(feishu)

set (CMAKE_CXX_FLAGS "-std=c++17 -Wall -DLLVM_USE_LINKER=lld -Wno-missing-braces")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")
add_compile_options(-Wno-subobject-linkage)

#---------------------------------------------------------------
# core

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../core)
  set(CORE ${CMAKE_CURRENT_SOURCE_DIR}/../core)
elseif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../core)
  set(CORE ${CMAKE_CURRENT_SOURCE_DIR}/../../core)
elseif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../core)
  set(CORE ${CMAKE_CURRENT_SOURCE_DIR}/../../../core)
endif()

message(STATUS "core: ${CORE}")
add_subdirectory(${CORE} core)
message(STATUS "Build with core source code 🆗")

#---------------------------------------------------------------
# SDK
include_directories(
    sdk/include
)

set(SDK
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/libcrypto.so.10
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/libsgitquotapi.so
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/libsgittradeapi.so
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/libssl.so.10
)

#---------------------------------------------------------------
# app
file(GLOB SOURCES "src/*.cpp" "main.cpp")
add_executable(${PROJECT_NAME}
  ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE core ${SDK})

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)