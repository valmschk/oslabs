cmake_minimum_required(VERSION 3.15)
project("Lab4_ProcessSync")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif()

include_directories(include)

add_executable(Receiver src/receiver.cpp)
add_executable(Sender src/sender.cpp)

set(OUTPUT_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
set_target_properties(Receiver PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BIN_DIR}")
set_target_properties(Sender PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BIN_DIR}")

enable_testing()
add_subdirectory(tests)