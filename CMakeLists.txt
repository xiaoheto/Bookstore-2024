cmake_minimum_required(VERSION 3.22)
project(Bookstore_2024)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(Bookstore_2024
    code
    src/main.cpp
    src/AccountManager.cpp
    src/AccountManager.h
    src/BlockLink.h
    src/BookManager.cpp
    src/BookManager.h
    src/CommandParser.cpp
    src/CommandParser.h
    src/LogManager.cpp
    src/LogManager.h
    src/MemoryRiver.h
        src/error.h)
