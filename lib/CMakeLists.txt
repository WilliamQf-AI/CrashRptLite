file(GLOB SOURCE_FILES 	./*.cpp ./*.h ./*.hpp)
file(GLOB HEADER_FILES  ./include/libWinCrashpad/*.h )

if(WINCRASHPAD_BUILD_SHARED_LIBS)
  list(APPEND SOURCE_FILES ./WinCrashpad.def)
endif()

if(WINCRASHPAD_BUILD_SHARED_LIBS)
  add_library(libWinCrashpad SHARED ${HEADER_FILES} ${SOURCE_FILES})
else()
  add_library(libWinCrashpad STATIC ${HEADER_FILES} ${SOURCE_FILES})
  
  target_compile_definitions(libWinCrashpad PRIVATE WINCRASHPAD_STATIC_LIB)
endif()

target_precompile_headers(libWinCrashpad PRIVATE StdAfx.h)

target_compile_definitions(libWinCrashpad PRIVATE UNICODE _UNICODE _CRT_SECURE_NO_DEPRECATE)

target_include_directories(libWinCrashpad
	PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/include>
	PUBLIC $<INSTALL_INTERFACE:./include>
)

if(WINCRASHPAD_BUILD_SHARED_LIBS)
	target_link_libraries(libWinCrashpad PRIVATE rpcrt4.lib version.lib dbghelp.lib)
else()
	target_link_libraries(libWinCrashpad PUBLIC rpcrt4.lib version.lib dbghelp.lib)
endif()

# set output name
set_target_properties(libWinCrashpad PROPERTIES 
	OUTPUT_NAME $<IF:$<BOOL:${WINCRASHPAD_BUILD_SHARED_LIBS}>,libWinCrashpad,libWinCrashpad-static>
	DEBUG_OUTPUT_NAME $<IF:$<BOOL:${WINCRASHPAD_BUILD_SHARED_LIBS}>,libWinCrashpad-d,libWinCrashpad-static-d>
)

install(TARGETS libWinCrashpad
  EXPORT winCrashpadConfig
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ./include/WinCrashpad DESTINATION include)

install(EXPORT winCrashpadConfig
  NAMESPACE libWinCrashpad::
  DESTINATION share/libWinCrashpad
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  winCrashpadConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/winCrashpadConfigVersion.cmake DESTINATION share/libWinCrashpad)
