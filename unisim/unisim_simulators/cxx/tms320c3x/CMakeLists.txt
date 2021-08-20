cmake_minimum_required ( VERSION 2.6 )
project (tms320c3x)
include (CheckIncludeFileCXX)
include (CheckIncludeFile)
	
check_include_file_cxx ( iostream HAVE_IOSTREAM )
if ( NOT ${HAVE_IOSTREAM} )
	message ( SEND_ERROR "Could not find c++ STL library \"iostream\"." )
endif ( NOT ${HAVE_IOSTREAM} )

check_include_file ( getopt.h HAVE_GETOPT_H )
if ( NOT ${HAVE_GETOPT_H} )
	message ( SEND_ERROR "Could not find \"getopt.h\"" )
endif ( NOT ${HAVE_GETOPT_H} )

check_include_file ( stdlib.h HAVE_STDLIB_H )
if ( NOT ${HAVE_STDLIB_H} )
	message ( SEND_ERROR "Could not find \"stdlib.h\"" )
endif ( NOT ${HAVE_STDLIB_H} )

check_include_file ( signal.h HAVE_SIGNAL_H )
if ( NOT ${HAVE_SIGNAL_H} )
	message ( SEND_ERROR "Could not find \"signal.h\"" )
endif ( NOT ${HAVE_SIGNAL_H} )

check_include_file_cxx ( stdexcept HAVE_STDEXCEPT )
if ( NOT ${HAVE_STDEXCEPT} )
	message ( SEND_ERROR "Could not find c++ STL library \"stdexcept\"." )
endif ( NOT ${HAVE_STDEXCEPT} )

add_subdirectory ( ${CMAKE_SOURCE_DIR}/unisim/kernel )
add_subdirectory ( ${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320 )
add_subdirectory ( ${CMAKE_SOURCE_DIR}/unisim/component/cxx/memory/ram )
add_subdirectory ( ${CMAKE_SOURCE_DIR}/unisim/service/time/host_time )
add_subdirectory ( ${CMAKE_SOURCE_DIR}/unisim/service/loader/coff_loader )
add_subdirectory ( ${CMAKE_SOURCE_DIR}/unisim/service/os/ti_c_io )
add_subdirectory ( ${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server )
add_subdirectory ( ${CMAKE_SOURCE_DIR}/unisim/service/debug/inline_debugger )

include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} )
add_executable ( tms320c3x main.cc )
target_link_libraries ( tms320c3x
	unisim__kernel__service
	unisim__component__cxx__processor__tms320
	unisim__component__cxx__memory__ram
	unisim__service__time__host_time
	unisim__service__loader__coff_loader
	unisim__service__os__ti_c_io
	unisim__service__debug__gdb_server
	unisim__service__debug__inline_debugger
	)
	
# stl_iostream ()
# clib_getopt_h ()
# clib_stdlib_h ()
# clib_signal_h ()
# stl_stdexcept ()
# unisim__kernel__service (UNISIM__KERNEL__SERVICE_SRCS
# 	UNISIM__KERNEL__SERVICE_INCDIR
#	UNISIM__KERNEL__SERVICE_LIBS
#	UNISIM__KERNEL__SERVICE_DEFS)
# unisim__component__cxx__processor__tms320 ()
# unisim__component__cxx__memory__ram ()
# unisim__service__time__host_time ()
# unisim__service__debug__gdb_server ()
# unisim__service__debug__inline_debugger ()
# unisim__service__loader__coff_loader ()
# unisim__service__os__ti_c_io ()
# win_windows_h ()
# win_winsock2_h ()

# add_executable (tms320c3x ${UNISIM__KERNEL__SERVICE_SRCS})
# include_directories (${UNISIM__KERNEL__SERVICE_INCDIR})
# add_definitions (${UNISIM__KERNEL__SERVICE_DEFS})
# target_link_libraries (tms320c3x ${UNISIM__KERNEL__SERVICE_LIBS})
