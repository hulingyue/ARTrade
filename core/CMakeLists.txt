cmake_minimum_required(VERSION 3.16)
project(core)

set (CMAKE_CXX_FLAGS "-std=c++20 -Wall -DLLVM_USE_LINKER=lld -Wno-missing-braces -Wno-unused-local-typedef")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")

file(
    GLOB 
    SOURCES 
    "src/*.cpp"
    "3rd/websocketpp/*.cpp"
)

file(GLOB HEADERS "include/*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_subdirectory(3rd/spdlog)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/pybind11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/nlohmann/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/websocketpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    spdlog
    SSL
    Crypto
)
