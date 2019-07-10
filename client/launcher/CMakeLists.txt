cmake_minimum_required(VERSION 3.0)
project(Launcher)

# Debug
add_definitions (-DDEBUG)

# Set C++17 standard
set (CMAKE_CXX_STANDARD 17)

# Get all the cpp and h files for the launcher
file(GLOB SOURCES "src/*.cpp" "src/*.h")

# Get all the cpp and h files from the shared
file(GLOB_RECURSE SHARED_SOURCES "../shared/*.cpp" "../shared/*.h")

# Compile the PolyHook library
set(SUBHOOK_STATIC ON)
set(SUBHOOK_INSTALL OFF)
set(SUBHOOK_TESTS OFF)
add_subdirectory(../vendor/subhook subhook)
include_directories(../vendor/subhook)

# Add the launcher as an executable to be compiled
add_executable(launcher WIN32 ${SOURCES} ${SHARED_SOURCES})

# Link the launcher to the dependencies' libs
target_link_libraries(launcher subhook)