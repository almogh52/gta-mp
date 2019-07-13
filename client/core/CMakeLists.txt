cmake_minimum_required(VERSION 3.0)
project(GTAMPCore)

# Debug
add_definitions (-DDEBUG)

# Set C++17 standard
set (CMAKE_CXX_STANDARD 17)

# Define DLL Export
add_definitions(-DGTAMPCORE_EXPORT)

# Get all the cpp and h files for the launcher
file(GLOB SOURCES "src/*.cpp" "src/*.h" "include/*.h")

# Get all header files from the shared
file(GLOB_RECURSE SHARED_SOURCES "../shared/*.h")

# Include headers for subhook
include_directories(../vendor/subhook)

# Include headers for CEF
get_filename_component(CEF_ROOT "../vendor/cef" ABSOLUTE)
include_directories(${CEF_ROOT})

# Add the launcher as an executable to be compiled
add_library(GTAMPCore SHARED ${SOURCES} ${SHARED_SOURCES})