cmake_minimum_required(VERSION 3.16)
project(core)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_subdirectory(3rd/spdlog)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/pybind11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/nlohmann/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/beast/include
)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system)

target_link_libraries(${PROJECT_NAME} PUBLIC
    spdlog
)
