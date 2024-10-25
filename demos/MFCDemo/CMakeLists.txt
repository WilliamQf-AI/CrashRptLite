file(GLOB SRC_FILES *.h *.cpp)

# Use MFC
if (NOT WINCRASHPAD_USE_STATIC_CRT)     					
	# Use MFC as a shared lib
    ADD_DEFINITIONS(-D_AFXDLL)
	SET(CMAKE_MFC_FLAG 2) 
else()   
	# Use MFC as a static lib
	SET(CMAKE_MFC_FLAG 1) 
endif()   

add_executable(MFCDemo 
	${SRC_FILES}
	MFCDemo.rc
)

target_compile_definitions(MFCDemo PRIVATE UNICODE _UNICODE)

target_include_directories(MFCDemo PRIVATE ${CMAKE_SOURCE_DIR}/lib/include)

target_precompile_headers(MFCDemo PRIVATE stdafx.h)

target_link_libraries(MFCDemo libWinCrashpad)

set_target_properties(MFCDemo PROPERTIES 
    OUTPUT_NAME MFCDemo
    DEBUG_OUTPUT_NAME MFCDemo-d
)

set_target_properties(MFCDemo PROPERTIES
	WIN32_EXECUTABLE TRUE
	VS_DPI_AWARE "PerMonitor"
)