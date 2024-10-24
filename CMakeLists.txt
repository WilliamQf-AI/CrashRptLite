cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH};")

project(WinCrashpad
  VERSION 1.0
  LANGUAGES CXX
)

if(NOT MSVC)
	message(FATAL_ERROR "WinCrashpad only support windows platform.")
endif()

# Build options
option(WINCRASHPAD_BUILD_SHARED_LIBS "If set (default), WinCrashpad are built as dynamic-link library, otherwise as static-link library." ON)
option(WINCRASHPAD_BUILD_DEMOS "Build demo projects." OFF)
option(WINCRASHPAD_USE_STATIC_CRT "Set to ON to build with static CRT on Windows (/MT)." OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WINCRASHPAD_USE_STATIC_CRT)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Other CMakeLists are located in project subdirectories 
add_subdirectory("lib")
add_subdirectory("exe")

add_subdirectory("thirdparty/tinyxml")
add_subdirectory("thirdparty/jpeg")
add_subdirectory("thirdparty/minizip")
add_subdirectory("thirdparty/zlib")
add_subdirectory("thirdparty/libpng")

if(WINCRASHPAD_BUILD_DEMOS)
  add_subdirectory("demos/ConsoleDemo")
  add_subdirectory("demos/MFCDemo")
endif()
