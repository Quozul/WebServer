cmake_minimum_required(VERSION 3.8)
project("WebServer")

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH "C:/Users/erwan/Documents/projects/cpp/lua")

if(NOT DEFINED LUA_INCLUDE_DIR OR NOT DEFINED LUA_LIBRARIES)
    find_package(Lua 5.3 REQUIRED)
endif()
include_directories(${LUA_INCLUDE_DIR})

add_executable(${PROJECT_NAME} src/main.cpp src/Response.cpp src/Response.hpp src/servlet.cpp src/servlet.hpp src/Request.cpp src/Request.hpp src/Queue.hpp src/luaFunctions.hpp src/luaFunctions.cpp src/socket.cpp src/socket.hpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL Threads::Threads ${LUA_LIBRARIES})

file(COPY server.conf DESTINATION .)