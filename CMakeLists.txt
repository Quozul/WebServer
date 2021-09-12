cmake_minimum_required(VERSION 3.16.3)
project(WebServer)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH "C:/Users/erwan/Documents/projects/cpp/lua")

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Werror")

if(NOT DEFINED LUA_INCLUDE_DIR OR NOT DEFINED LUA_LIBRARIES)
    find_package(Lua 5.3 REQUIRED)
endif()
include_directories(${LUA_INCLUDE_DIR})

add_executable(${PROJECT_NAME} src/main.cpp src/Response.cpp src/Response.h src/servlet.cpp src/servlet.h src/Request.cpp src/Request.h src/Queue.h)

target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL Threads::Threads)