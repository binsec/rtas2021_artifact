stl_iostream ()
clib_getopt_h ()
clib_stdlib_h ()
clib_signal_h ()
stl_stdexcept ()
unisim__kernel__service (UNISIM__KERNEL__SERVICE_SRCS
	UNISIM__KERNEL__SERVICE_INCDIR
	UNISIM__KERNEL__SERVICE_LIBS
	UNISIM__KERNEL__SERVICE_DEFS)
# unisim__component__cxx__processor__tms320 ()
# unisim__component__cxx__memory__ram ()
# unisim__service__time__host_time ()
# unisim__service__debug__gdb_server ()
# unisim__service__debug__inline_debugger ()
# unisim__service__loader__coff_loader ()
# unisim__service__os__ti_c_io ()
# win_windows_h ()
# win_winsock2_h ()

add_executable (tms320c3x ${UNISIM__KERNEL__SERVICE_SRCS})
include_directories (${UNISIM__KERNEL__SERVICE_INCDIR})
add_definitions (${UNISIM__KERNEL__SERVICE_DEFS})
target_link_libraries (tms320c3x ${UNISIM__KERNEL__SERVICE_LIBS})
