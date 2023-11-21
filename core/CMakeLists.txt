cmake_minimum_required(VERSION 3.16)
project(core)

set (CMAKE_CXX_FLAGS "-std=c++17 -Wall -DLLVM_USE_LINKER=lld -Wno-missing-braces")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_subdirectory(3rd/spdlog)
add_subdirectory(3rd/pybind11)
add_subdirectory(3rd/nlohmann)
add_subdirectory(3rd/CLI11)
add_subdirectory(3rd/websocketpp)
add_subdirectory(3rd/cpp-httplib)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/pybind11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/nlohmann/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/CLI11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/websocketpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/cpp-httplib
)

find_package(OpenSSL REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC
    spdlog
    CLI11
    OpenSSL::SSL
    OpenSSL::Crypto
)

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include/${PROJECT_NAME})
