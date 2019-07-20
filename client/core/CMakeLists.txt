cmake_minimum_required(VERSION 3.0)
project(GTAMPCore)

# Debug
add_definitions (-DDEBUG)

# Set C++17 standard
set (CMAKE_CXX_STANDARD 17)

# Define DLL Export
add_definitions(-DGTAMPCORE_EXPORT)

# Get all the cpp and h files for the launcher
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "include/*.h")

# Get all source and header files from the shared
file(GLOB_RECURSE SHARED_SOURCES "../shared/*.cpp" "../shared/*.h")

# Change runtime library to MT
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MT")

# Compile the subhook library
set(SUBHOOK_STATIC OFF)
set(SUBHOOK_INSTALL OFF)
set(SUBHOOK_TESTS OFF)
add_subdirectory(../vendor/subhook subhook)
include_directories(../vendor/subhook)

# Compile the discord-rpc library
set(BUILD_EXAMPLES OFF)
set(USE_STATIC_CRT ON)
add_subdirectory(../vendor/discord-rpc discord-rpc)
include_directories(../vendor/discord-rpc/include)

# Compile the spdlog library
add_subdirectory(../../vendor/spdlog spdlog)
include_directories(../../vendor/spdlog/include)

# Bundle the CEF as well
set(CEF_CXX_COMPILER_FLAGS_DEBUG "/MT")
get_filename_component(CEF_ROOT "../vendor/cef" ABSOLUTE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
include_directories(${CEF_ROOT})
find_package(CEF REQUIRED)
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Add the launcher as an executable to be compiled
add_library(GTAMPCore SHARED ${SOURCES} ${SHARED_SOURCES})

# Link the core library to the dependencies' libs
target_link_libraries(GTAMPCore subhook discord-rpc libcef_dll_wrapper spdlog)