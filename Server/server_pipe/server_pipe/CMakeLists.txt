# CMakeList.txt : CMake project for server_pipe, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (server_pipe "SyncServer.cpp"
            "StringOperations.cpp"
            "ServerPipe.cpp"
            "pch.cpp"
            "AsyncServer.cpp"
             "MathOperations.cpp"
            "CustomClass.cpp"
            "Utils.h"
            "SyncServer.h"
            "StringOperations.h"
            "pch.h"
            "AsyncServer.h"
            "MathOperations.h"
            "Blocking_Queue.h"
            "CustomClass.h")

# TODO: Add tests and install targets if needed.
