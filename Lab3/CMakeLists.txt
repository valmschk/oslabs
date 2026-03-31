cmake_minimum_required(VERSION 3.10)
project(Lab3_Threads)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)


add_executable(ThreadSync 
    src/main.cpp 
    src/Array.cpp 
    src/Marker.cpp
)


add_executable(RunTests 
    tests/test_logic.cpp 
    src/Array.cpp
)