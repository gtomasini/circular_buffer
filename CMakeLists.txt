cmake_minimum_required (VERSION 3.9.1)
project (RingBuffer)
include(CMakePrintHelpers)
cmake_print_variables(PROJECT_SOURCE_DIR)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)
find_package (Threads)
add_executable (circular src/Module1.cpp src/Module2.cpp src/Module3.cpp src/unittests.cpp  src/main.cpp)
target_link_libraries (circular ${CMAKE_THREAD_LIBS_INIT})

