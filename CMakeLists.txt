cmake_minimum_required(VERSION 3.26)
project(battlegame_server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-stdlib=libc++")

add_executable(battlegame_server main.cpp
        Socket.cpp
        Socket.h
        Wsa.cpp
        Wsa.h)
