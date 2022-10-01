cmake_minimum_required(VERSION 3.8)
project("WebServer")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

if(NOT DEFINED LUA_INCLUDE_DIR OR NOT DEFINED LUA_LIBRARIES)
    find_package(Lua 5.4 REQUIRED)
endif()
include_directories(${LUA_INCLUDE_DIR})

# Import thirdparty libraries
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/json/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/json/external/PEGTL/include)
add_subdirectory("thirdparty/json")

file(GLOB_RECURSE CONFIGURE_DEPENDS "src/*.cpp")
add_executable(${PROJECT_NAME} ${CONFIGURE_DEPENDS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL Threads::Threads ${LUA_LIBRARIES})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)