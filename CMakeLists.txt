cmake_minimum_required(VERSION 3.26)
project(battlegame_server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-stdlib=libc++")

add_executable(battlegame_server main.cpp
        Socket.cpp
        Socket.h
        Wsa.cpp
        Wsa.h
        Message.cpp
        Message.h
        Client.cpp
        Client.h
        Constants.h
        StcRpc.h
        StcRpc.cpp
        CtsRpc.h
        CtsRpc.cpp
        Context.h
        Context.cpp
        IoResult.h
        GameRoom.h
        GameRoom.cpp)
