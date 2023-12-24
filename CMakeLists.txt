cmake_minimum_required(VERSION 3.27)
project(web_server_reboot)

set(CMAKE_CXX_STANDARD 20)

add_executable(web_server_reboot main.cpp
        App.cpp
        App.h
        connections/Connection.cpp
        connections/Connection.h
        connections/SocketConnection.cpp
        connections/SocketConnection.h)
