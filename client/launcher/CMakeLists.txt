cmake_minimum_required(VERSION 3.0)
project(GTAMPLauncher)

# Debug
add_definitions (-DDEBUG)

# Set C++17 standard
set (CMAKE_CXX_STANDARD 17)

# Get all the cpp and h files for the launcher
file(GLOB SOURCES "src/*.cpp" "src/*.h")

# Get all the cpp and h files from the shared
file(GLOB_RECURSE SHARED_SOURCES "../shared/*.cpp" "../shared/*.h")

# Change runtime library to MT
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MT")

# Compile the core library
add_subdirectory(../core GTAMPCore)
include_directories(../core/include)

# Compile the subhook library
set(SUBHOOK_STATIC OFF)
set(SUBHOOK_INSTALL OFF)
set(SUBHOOK_TESTS OFF)
add_subdirectory(../vendor/subhook subhook)
include_directories(../vendor/subhook)

# Bundle the CEF as well
get_filename_component(CEF_ROOT "../vendor/cef" ABSOLUTE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
set(CEF_LIB "${CEF_ROOT}/Release/libcef.lib")
include_directories(${CEF_ROOT})
find_package(CEF REQUIRED)
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Add the launcher as an executable to be compiled
add_executable(GTAMPLauncher WIN32 ${SOURCES} ${SHARED_SOURCES})

# Link the launcher to the dependencies' libs
target_link_libraries(GTAMPLauncher GTAMPCore subhook libcef_dll_wrapper ${CEF_LIB})

# Copy GTAMPCore DLL to launcher dir
add_custom_command(TARGET GTAMPLauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTAMPCore> $<TARGET_FILE_DIR:GTAMPLauncher>
)

# Copy subhook DLL to launcher dir
add_custom_command(TARGET GTAMPLauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:subhook> $<TARGET_FILE_DIR:GTAMPLauncher>
)

# Copy all cef release files
file(GLOB_RECURSE CEF_RELEASE_FILES ABSOLUTE "${CEF_ROOT}/Release/*.dll" "${CEF_ROOT}/Release/*.bin")
foreach (FILE_PATH ${CEF_RELEASE_FILES})
file(RELATIVE_PATH FILE "${CEF_ROOT}/Release" ${FILE_PATH}) # Keep track of sub-folders
	set(DEST_FILE "$<TARGET_FILE_DIR:GTAMPLauncher>/${FILE}")
	add_custom_command(TARGET GTAMPLauncher POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FILE_PATH} ${DEST_FILE}
	)
endforeach()

# Copy all cef resources files
file(GLOB_RECURSE CEF_RESOURCES_FILES ABSOLUTE "${CEF_ROOT}/Resources/*")
foreach (FILE_PATH ${CEF_RESOURCES_FILES})
	file(RELATIVE_PATH FILE "${CEF_ROOT}/Resources" ${FILE_PATH}) # Keep track of sub-folders
	set(DEST_FILE "$<TARGET_FILE_DIR:GTAMPLauncher>/${FILE}")
	add_custom_command(TARGET GTAMPLauncher POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FILE_PATH} ${DEST_FILE}
	)
endforeach()