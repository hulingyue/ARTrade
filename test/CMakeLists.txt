cmake_minimum_required(VERSION 3.16)
project(websocketpp_demo)

set(CMAKE_CXX_STANDARD 11)

# 查找Boost库
find_package(Boost REQUIRED COMPONENTS system)

# ... ... ... 添加可执行文件
add_executable(websocketpp_demo main.cpp)

# 设置包含路径
target_include_directories(websocketpp_demo PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../core/3rd/websocketpp)

# 链接所需的库
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
