cmake_minimum_required (VERSION 3.5)
project (RingBuffer)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)
find_package (Threads)
add_executable (circular main.cpp module.cpp unittests.cpp)
target_link_libraries (circular ${CMAKE_THREAD_LIBS_INIT})

