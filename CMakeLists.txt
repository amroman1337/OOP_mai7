cmake_minimum_required(VERSION 3.10)

project(laba7)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(laba7 
    main.cpp 
    npc.cpp 
    druid.cpp 
    elf.cpp 
    black_knight.cpp
    fight_manager.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(laba7 Threads::Threads)