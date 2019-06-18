cmake_minimum_required(VERSION 3.4)
project(q3dobserver-project)

# Define build options:
option(Q3DO_BUILD_ALL "Build all q3dobserver artefacts" OFF)
option(Q3DO_BUILD_TESTS "Build q3dobserver tests" OFF)

# Enable C++11:
set(CMAKE_CXX_STANDARD 11)

# Detect Clang:
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

# Set compiler warnings
# (https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#compilers)
# (https://stackoverflow.com/questions/3375697/useful-gcc-flags-for-c)
# (https://stackoverflow.com/questions/5088460/flags-to-enable-thorough-and-verbose-g-warnings):
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANGXX)
    add_definitions("-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wcast-align -Wcast-qual -Wunused
	-Woverloaded-virtual -Wpedantic -Wdouble-promotion -Wformat=2 -Wfloat-equal -Wpointer-arith
	-Wwrite-strings -Wswitch-enum -Wunreachable-code -Winit-self -Wuninitialized
	-Wno-unused-result -Wctor-dtor-privacy -Wdisabled-optimization -Wmissing-declarations
        -Wredundant-decls -Wno-unused")
endif()

# Add library subdirectory:
add_subdirectory(lib)

# Add tests subdirectory:
if(Q3DO_BUILD_TESTS OR Q3DO_BUILD_ALL)
    add_subdirectory(tests)
endif()
