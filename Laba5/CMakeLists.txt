cmake_minimum_required(VERSION 3.20)
project(NamedPipesLab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(common)

add_executable(Server Server/Server.cpp common/employee.h)
add_executable(Client Client/Client.cpp common/employee.h)
add_executable(UnitTests tests/test_suite.cpp common/employee.h)

enable_testing()
add_test(NAME ComprehensiveTest COMMAND UnitTests)