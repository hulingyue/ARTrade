cmake_minimum_required(VERSION 3.16)
project(core)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp" "src/websocket/*.cpp")
file(GLOB HEADERS "include/*.h" "include/websocket/*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_subdirectory(3rd/spdlog)
add_subdirectory(3rd/websocketpp)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/websocket
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/pybind11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/nlohmann/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    spdlog
)
