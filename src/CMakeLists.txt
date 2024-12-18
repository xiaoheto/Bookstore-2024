cmake_minimum_required(VERSION 3.22)
project(bin)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(bin
        AccountManager.cpp
        AccountManager.h
        LogManager.h
        LogManager.cpp
        CommandPaser.h
        CommandPaser.cpp
        error.h
        MemoryPool.h
        BookManager.h
        BookManager.cpp
        main.cpp
        BlockLink.h)