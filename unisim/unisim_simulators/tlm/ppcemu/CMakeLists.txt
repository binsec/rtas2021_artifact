cmake_minimum_required ( VERSION 2.8 )
project ( unisim_ppcemu )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CMAKE_CXX_SOURCE_FILE_EXTENSIONS *.cc *.tcc *.hh CACHE STRING "Extensions of the source files" )
	
set ( CXX_INCLUDES iostream stdexcept )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

if ( WIN32 )
	set ( C_INCLUDES getopt stdlib signal windows winsock2 )
else ( WIN32 )
	set ( C_INCLUDES getopt stdlib signal )
endif ( WIN32 )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

add_definitions ( -DSC_INCLUDE_DYNAMIC_PROCESSES )

set ( ppcemu_subdirs
	unisim/kernel
	unisim/component/tlm/processor/powerpc/mpc7447a
	unisim/component/tlm/memory/ram
	unisim/component/tlm/fsb/snooping_bus
	unisim/component/cxx/processor/powerpc
	unisim/component/tlm/debug
	unisim/component/tlm/bridge/snooping_fsb_to_mem
	unisim/service/debug/gdb_server
	unisim/service/debug/inline_debugger
	unisim/service/loader/elf_loader
	unisim/service/loader/linux_loader
	unisim/service/os/linux_os
	unisim/service/power
	unisim/service/time/host_time
	unisim/service/time/sc_time
	unisim/util/garbage_collector
	)
foreach ( dir ${ppcemu_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${OSCI_SYSTEMC_INCLUDE_DIR} )

file ( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/gdb_server )
file ( 
	COPY 
		${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_armv4l.xml 
	DESTINATION 
		${CMAKE_BINARY_DIR}/resources/gdb_server )

add_executable ( ppcemu 
	${CMAKE_SOURCE_DIR}/main.cc )
target_link_libraries ( ppcemu
	${OSCI_SYSTEMC_LIBRARIES}
	unisim__kernel__service
	unisim__component__tlm__processor__powerpc__mpc7447a
	unisim__component__tlm__memory__ram
	unisim__component__tlm__fsb__snooping_bus
	unisim__component__tlm__bridge__snooping_fsb_to_mem
	unisim__service__debug__gdb_server
	unisim__service__debug__inline_debugger
	unisim__service__loader__elf_loader
	unisim__service__loader__linux_loader
	unisim__service__os__linux_os
	unisim__service__power
	unisim__service__time__host_time
	unisim__service__time__sc_time
	unisim__util__garbage_collector
	)

add_dependencies ( ppcemu
	unisim__kernel__service
	unisim__component__tlm__processor__powerpc__mpc7447a
	unisim__component__tlm__memory__ram
	unisim__component__tlm__fsb__snooping_bus
	unisim__component__cxx__processor__powerpc
	unisim__component__tlm__debug
	unisim__component__tlm__bridge__snooping_fsb_to_mem
	unisim__service__debug__gdb_server
	unisim__service__debug__inline_debugger
	unisim__service__loader__elf_loader
	unisim__service__loader__linux_loader
	unisim__service__power
	unisim__service__os__linux_os
	unisim__service__time__host_time
	unisim__service__time__sc_time
	unisim__util__garbage_collector
	)

set_target_properties ( ppcemu
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

set ( SIM_VERSION_MAJOR 0 )
set ( SIM_VERSION_MINOR 1 )
set ( SIM_VERSION_PATCH 0 )
set ( SIM_VERSION_CODENAME "\"Orsay\"" )
set ( SIM_AUTHOR "\"Gilles Mouchard (gilles.mouchard@cea.fr)\"" )
# configure_file ( 
# 	${CMAKE_SOURCE_DIR}/config.hh.in 
# 	${CMAKE_BINARY_DIR}/config.hh )

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "3")
set(CPACK_SOURCE_GENERATOR "TBZ2")
include ( CPack )


