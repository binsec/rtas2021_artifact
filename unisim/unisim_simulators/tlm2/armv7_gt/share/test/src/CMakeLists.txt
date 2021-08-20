cmake_minimum_required ( VERSION 2.8 ) 
project ( test_static )
include ( CheckIncludeFile )

if ( CMAKE_TOOLCHAIN_FILE )
	message ( STATUS "Using ${CMAKE_TOOLCHAIN_FILE}" )
	set ( TARGET_NAME test.armv5l )
else ( CMAKE_TOOLCHAIN_FILE )
	message ( STATUS "Not crosscompiling" )
	set ( TARGET_NAME test )
endif ( CMAKE_TOOLCHAIN_FILE )
# if ( CMAKE_CROSSCOMPILING )
# 	set ( TARGET_NAME test.armv5l )
# else ( CMAKE_CROSSCOMPILING )
# 	set ( TARGET_NAME test )
# endif ( CMAKE_CROSSCOMPILING )

add_executable ( ${TARGET_NAME}
	main.c )

# if ( CMAKE_CROSSCOMPILING )
if ( CMAKE_TOOLCHAIN_FILE )
	# set the following line to avoid any shared/dynamic command during the link
	SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
	set_target_properties ( ${TARGET_NAME}
		PROPERTIES
			COMPILE_FLAGS   "-g3 -O0 -march=armv5 -mlittle-endian -msoft-float -mno-thumb-interwork"
			LINK_FLAGS      "-static -march=armv5 -mlittle-endian"
	)
	install ( TARGETS ${TARGET_NAME}
		RUNTIME DESTINATION .
	)
# endif ( CMAKE_CROSSCOMPILING )
endif ( CMAKE_TOOLCHAIN_FILE )
