cmake_minimum_required(VERSION 3.16)
project(core)

set(CMAKE_CXX_STANDARD 17)

file(
    GLOB 
    SOURCES 
    PUBLIC 
    "src/*.cpp"
)

file(
    GLOB
    HEADERS
    PUBLIC
    "include/*.h"
)

add_library(
    ${PROJECT_NAME} 
    STATIC
    ${SOURCES}
)

add_subdirectory(3rd/spdlog)
add_subdirectory(3rd/pybind11)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/spdlog
  ${CMAKE_CURRENT_SOURCE_DIR}/3rd/pybind11
)

set(Boost_INCLUDE_DIR /usr/include/boost/include)
set(Boost_LIBRARY_DIR /usr/include/boost/lib)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})


find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(${PROJECT_NAME} PUBLIC
  spdlog
  pybind11
  Boost::system
  pthread
)
