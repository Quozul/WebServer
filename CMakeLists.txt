cmake_minimum_required(VERSION 3.27)
project(web_server_reboot)

set(CMAKE_CXX_STANDARD 20)

add_executable(web_server_reboot main.cpp
        App.cpp
        App.h
        connections/Connection.h
        connections/SocketConnection.cpp
        connections/SocketConnection.h
        responses/Response.cpp
        responses/Response.h
        responses/Request.cpp
        responses/Request.h
        SslHelpers.cpp
        SslHelpers.h
        connections/SslConnection.cpp
        connections/SslConnection.h)

find_package(OpenSSL REQUIRED)
target_link_libraries(web_server_reboot OpenSSL::SSL)
