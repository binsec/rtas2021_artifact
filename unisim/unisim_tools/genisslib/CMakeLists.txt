INCLUDE ( FindCygwin )

FIND_PROGRAM ( LEX_EXECUTABLE
	NAMES 
		flex
		lex
	PATH
		${CYGWIN_INSTALL_PATH}/bin
		/bin
		/usr/bin 
		/usr/local/bin
		/sbin
	)
MARK_AS_ADVANCED ( LEX_EXECUTABLE )

FIND_PROGRAM ( YACC_EXECUTABLE
	NAMES 
		bison
		yacc
	PATH
		${CYGWIN_INSTALL_PATH}/bin
		/bin
		/usr/bin 
		/usr/local/bin
		/sbin
	)
MARK_AS_ADVANCED ( YACC_EXECUTABLE )

message ( STATUS "Checking unisim_tools::genisslib" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )

set ( CXX_INCLUDES limits map memory vector iosfwd iostream fstream ostream cstdio cstdlib cassert cctype cerrno cstdarg cstring )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES inttypes unistd )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

# ADD_LEX_FILES ( ${CMAKE_SOURCE_DIR}/scanner.ll )
# ADD_YACC_FILES ( ${CMAKE_SOURCE_DIR}/parser.yy )

add_custom_command (
	OUTPUT 
		${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
	COMMAND 
		${LEX_EXECUTABLE} # flex
			-l -o${CMAKE_CURRENT_BINARY_DIR}/scanner.cc ${CMAKE_SOURCE_DIR}/genisslib/scanner.ll
	DEPENDS 
		${CMAKE_SOURCE_DIR}/genisslib/scanner.ll
	COMMENT "Creating GenISSLib scanner"
	)

add_custom_command (
	OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/parser.h
		${CMAKE_CURRENT_BINARY_DIR}/parser.cc
	COMMAND
		${YACC_EXECUTABLE} -y -d -p yy -o${CMAKE_CURRENT_BINARY_DIR}/parser.c ${CMAKE_SOURCE_DIR}/genisslib/parser.yy
	COMMAND
		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/parser.c ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
	COMMAND
		${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/parser.c
	DEPENDS
		${CMAKE_SOURCE_DIR}/genisslib/parser.yy
	COMMENT "Creating GenISSLib parser"
	)

set ( GENISSLIB_SRCS 
	${CMAKE_SOURCE_DIR}/genisslib/action.hh
	${CMAKE_SOURCE_DIR}/genisslib/conststr.hh
	${CMAKE_SOURCE_DIR}/genisslib/main.hh
	${CMAKE_SOURCE_DIR}/genisslib/riscgenerator.hh
	${CMAKE_SOURCE_DIR}/genisslib/subdecoder.hh
	${CMAKE_SOURCE_DIR}/genisslib/bitfield.hh
	${CMAKE_SOURCE_DIR}/genisslib/errtools.hh
	${CMAKE_SOURCE_DIR}/genisslib/operation.hh
	${CMAKE_SOURCE_DIR}/genisslib/scanner.hh
	${CMAKE_SOURCE_DIR}/genisslib/variable.hh
	${CMAKE_SOURCE_DIR}/genisslib/ciscgenerator.hh
	${CMAKE_SOURCE_DIR}/genisslib/fwd.hh
	${CMAKE_SOURCE_DIR}/genisslib/parser.hh
	${CMAKE_SOURCE_DIR}/genisslib/sourcecode.hh
	${CMAKE_SOURCE_DIR}/genisslib/vect.hh
	${CMAKE_SOURCE_DIR}/genisslib/cli.hh
	${CMAKE_SOURCE_DIR}/genisslib/generator.hh
	${CMAKE_SOURCE_DIR}/genisslib/product.hh
	${CMAKE_SOURCE_DIR}/genisslib/specialization.hh
	${CMAKE_SOURCE_DIR}/genisslib/comment.hh
	${CMAKE_SOURCE_DIR}/genisslib/isa.hh
	${CMAKE_SOURCE_DIR}/genisslib/referencecounting.hh
	${CMAKE_SOURCE_DIR}/genisslib/strtools.hh
	${CMAKE_SOURCE_DIR}/genisslib/action.cc
	${CMAKE_SOURCE_DIR}/genisslib/comment.cc
	${CMAKE_SOURCE_DIR}/genisslib/isa.cc
	${CMAKE_SOURCE_DIR}/genisslib/referencecounting.cc
	${CMAKE_SOURCE_DIR}/genisslib/strtools.cc
	${CMAKE_SOURCE_DIR}/genisslib/bitfield.cc
	${CMAKE_SOURCE_DIR}/genisslib/conststr.cc
	${CMAKE_SOURCE_DIR}/genisslib/main.cc
	${CMAKE_SOURCE_DIR}/genisslib/riscgenerator.cc
	${CMAKE_SOURCE_DIR}/genisslib/subdecoder.cc
	${CMAKE_SOURCE_DIR}/genisslib/ciscgenerator.cc
	${CMAKE_SOURCE_DIR}/genisslib/errtools.cc
	${CMAKE_SOURCE_DIR}/genisslib/operation.cc
	${CMAKE_SOURCE_DIR}/genisslib/sourcecode.cc
	${CMAKE_SOURCE_DIR}/genisslib/variable.cc
	${CMAKE_SOURCE_DIR}/genisslib/cli.cc
	${CMAKE_SOURCE_DIR}/genisslib/generator.cc
	${CMAKE_SOURCE_DIR}/genisslib/product.cc
	${CMAKE_SOURCE_DIR}/genisslib/specialization.cc
	${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
	${CMAKE_CURRENT_BINARY_DIR}/parser.h
	${CMAKE_CURRENT_BINARY_DIR}/parser.cc
	)

set_source_files_properties ( 
		${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
		${CMAKE_CURRENT_BINARY_DIR}/parser.h
		${CMAKE_CURRENT_BINARY_DIR}/parser.cc
	PROPERTIES
		GENERATED TRUE )

include_directories ( ${CMAKE_SOURCE_DIR}/genisslib ${CMAKE_CURRENT_BINARY_DIR} )
add_executable ( genisslib ${GENISSLIB_SRCS} )

message ( STATUS "Checking unisim_tools::genisslib - done" )



