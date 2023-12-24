cmake_minimum_required(VERSION 3.22)
project(web_server_reboot)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_executable(web_server_reboot main.cpp ${SOURCES}
        src/string_manipulation.cpp
        src/string_manipulation.h)

find_package(OpenSSL REQUIRED)
target_link_libraries(web_server_reboot OpenSSL::SSL)


find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()

    file(GLOB_RECURSE SOURCES "test/*.cpp" "src/*.cpp" "src/*.h")

    add_executable(web_server_reboot_test ${SOURCES}
            test/request_test.cpp)

    target_link_libraries(
            web_server_reboot_test
            GTest::gtest_main
            OpenSSL::SSL
    )

    include(GoogleTest)
    gtest_discover_tests(web_server_reboot_test)
else()
    message(WARNING "GTest not found. Tests will not be built.")
endif()
