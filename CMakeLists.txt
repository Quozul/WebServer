cmake_minimum_required(VERSION 3.8)
project("WebServer")

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH "C:/Users/erwan/Documents/projects/cpp/lua")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

if(NOT DEFINED LUA_INCLUDE_DIR OR NOT DEFINED LUA_LIBRARIES)
    find_package(Lua 5.3 REQUIRED)
endif()
include_directories(${LUA_INCLUDE_DIR})

# Import thirdparty libraries
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/json/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/json/external/PEGTL/include)
add_subdirectory("thirdparty/json")

file(GLOB_RECURSE CONFIGURE_DEPENDS "src/*.cpp")
add_executable(${PROJECT_NAME} ${CONFIGURE_DEPENDS} "src/utils.cpp")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL Threads::Threads ${LUA_LIBRARIES})
