cmake_minimum_required (VERSION 2.8.11)
project (RingBuffer)
set (CMAKE_CXX_STANDARD 17)
find_package (Threads)
add_executable (circular main.cpp module.cpp unittests.cpp)
target_link_libraries (circular ${CMAKE_THREAD_LIBS_INIT})

