cmake_minimum_required(VERSION 3.15)

add_executable(Tests test.cpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_disable_pthreads ON CACHE BOOL "Disable pthreads for GTest" FORCE)
FetchContent_MakeAvailable(googletest)

target_include_directories(Tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(Tests PRIVATE
    GTest::gtest
    GTest::gtest_main
)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL" CACHE STRING "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_test(NAME GTest_RingBuffer COMMAND Tests)