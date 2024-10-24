file(GLOB SOURCE_FILES 	./*.cpp ./*.h ./*.hpp)

add_executable(WinCrashpad
    ${SOURCE_FILES}
    Resource.rc
    ${CMAKE_SOURCE_DIR}/lib/SharedMem.cpp
)

target_compile_definitions(WinCrashpad PRIVATE UNICODE _UNICODE)

target_precompile_headers(WinCrashpad PRIVATE stdafx.h)

target_include_directories(WinCrashpad PRIVATE
                            ${CMAKE_SOURCE_DIR}/lib/include 
                            ${CMAKE_SOURCE_DIR}/lib
                            ${CMAKE_SOURCE_DIR}/thirdparty/zlib
                            ${CMAKE_SOURCE_DIR}/thirdparty/minizip  
                            ${CMAKE_SOURCE_DIR}/thirdparty/jpeg 
                            ${CMAKE_SOURCE_DIR}/thirdparty/libpng 
                            ${CMAKE_SOURCE_DIR}/thirdparty/tinyxml)

# Add input link libraries
target_link_libraries(WinCrashpad PRIVATE
    minizip libjpeg libpng tinyxml Rpcrt4.lib Gdi32.lib shell32.lib version.lib psapi.lib)

set_target_properties(WinCrashpad PROPERTIES COMPILE_FLAGS "/Os")

set_target_properties(WinCrashpad PROPERTIES 
    OUTPUT_NAME WinCrashpad
    DEBUG_OUTPUT_NAME WinCrashpad-d
)

set_property(TARGET WinCrashpad PROPERTY VS_DPI_AWARE "PerMonitor")

install(TARGETS WinCrashpad)