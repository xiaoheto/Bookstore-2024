cmake_minimum_required(VERSION 3.22)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(src
    Account.cpp
    Account.h
    Book.cpp
    Book.h
    CommandPaser.cpp
    CommandPaser.h
    Log.cpp
    Log.h
    main.cpp
    Utils.cpp
    Utils.h)
