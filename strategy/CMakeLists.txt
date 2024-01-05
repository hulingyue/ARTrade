cmake_minimum_required(VERSION 3.16)
project(strategy)

set (CMAKE_CXX_FLAGS "-std=c++17 -Wall -DLLVM_USE_LINKER=lld -Wno-missing-braces")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")
add_compile_options(-Wno-subobject-linkage)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#---------------------------------------------------------------
# pybind11

file(GLOB PYSTRATEGY_SOURCES "PythonApi/pystrategy/*.cpp")
pybind11_add_module("pystrategy" MODULE ${PYSTRATEGY_SOURCES})
target_link_libraries("pystrategy" PRIVATE core)
file(GLOB PYCORE_SOURCES "PythonApi/pycore/*.cpp")
pybind11_add_module("pycore" MODULE ${PYCORE_SOURCES})
target_link_libraries("pycore" PRIVATE core)
