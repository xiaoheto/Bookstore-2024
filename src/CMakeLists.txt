cmake_minimum_required(VERSION 3.22)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(code src/main.cpp
    AccountManager.cpp
    AccountManager.h
    BlockLink.h
    BookManager.cpp
    BookManager.h
    CommandParser.cpp
    CommandParser.h
    error.h
    LogManager.cpp
    LogManager.h
    MemoryRiver.h)
