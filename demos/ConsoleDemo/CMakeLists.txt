add_executable(ConsoleDemo 
    ConsoleDemo.cpp
)

target_compile_definitions(ConsoleDemo PRIVATE UNICODE _UNICODE)

target_include_directories(ConsoleDemo PRIVATE ${CMAKE_SOURCE_DIR}/lib/include)

target_link_libraries(ConsoleDemo PRIVATE libWinCrashpad)

set_target_properties(ConsoleDemo PROPERTIES 
    OUTPUT_NAME ConsoleDemo
    DEBUG_OUTPUT_NAME ConsoleDemo-d
)

set_target_properties(ConsoleDemo PROPERTIES
	VS_DPI_AWARE "PerMonitor"
)