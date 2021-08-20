## UNISIM_CHECK_LIBSTDCXX
## Checks if the stdc++ library is available
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBSTDCXX], [
	# Note: we can't check libstdc++ functions from libstdc++ because it's a library of C++ classes with no C functions.
	LIBS="-lstdc++${LIBS}"
])

## UNISIM_WITH_BOOST
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_WITH_BOOST], [
    # Check if boost path has been overloaded
    AC_ARG_WITH(boost,
	AS_HELP_STRING([--with-boost=<path>], [boost library to use (will be completed with /include and /lib)]))
    if test "x$with_boost" != "x"; then
	AC_MSG_NOTICE([using boost at $with_boost])
	CPPFLAGS=${CPPFLAGS}" -I$with_boost/include"
	LDFLAGS=${LDFLAGS}" -L$with_boost/lib"
    fi
])

## UNISIM_CHECK_BOOST_ARRAY
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_ARRAY], [
    # Check for some boost graph headers
    AC_CHECK_HEADERS([boost/array.hpp],,\
	AC_MSG_ERROR([boost array headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_BOOST_GRAPH
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_GRAPH], [
    # Check for some boost graph headers
    AC_CHECK_HEADERS([boost/graph/adjacency_list.hpp boost/graph/topological_sort.hpp boost/graph/visitors.hpp],,\
	AC_MSG_ERROR([boost graph headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_BOOST_THREAD
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_THREAD], [
    # Check for boost thread headers
    AC_CHECK_HEADERS([boost/thread/thread.hpp boost/thread/mutex.hpp boost/thread/xtime.hpp],,\
	AC_MSG_ERROR([boost thread headers not found. Please install the boost thread development library. Use --with-boost=<path> to overload default includes search path.]))

	# Note: we can't check libboost_thread-mt functions from libboost_thread-mt because it's a library of C++ classes with no C functions.
	LIBS="-lboost_thread-mt ${LIBS}"
])

## UNISIM_CHECK_BOOST_FUNCTION
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_FUNCTION], [
    # Check for boost function header
    AC_CHECK_HEADERS([boost/function.hpp],,\
	AC_MSG_ERROR([boost function header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_BOOST_MEM_FN
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_MEM_FN], [
    # Check for mem_fn header
    AC_CHECK_HEADERS([boost/mem_fn.hpp],,\
	AC_MSG_ERROR([boost mem_fn header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_BOOST_BIND
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_BIND], [
    # Check for boost bind
    AC_CHECK_HEADERS([boost/bind.hpp],,\
	AC_MSG_ERROR([boost bind header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_WINSOCK2
## Checks if the winsock2 library is available
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_WINSOCK2], [
	# Note: we can't check socket functions from libwsock32 because of the PASCAL calling convention. cdecl is mandatory for autoconf.
	LIBS="-lwsock32 ${LIBS}"
])

## UNISIM_CHECK_CURSES
## Checks if the curses library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_CURSES], [
    # Check if curses path has been overloaded
    AC_ARG_WITH(ncurses,
	AS_HELP_STRING([--with-ncurses=<path>], [ncurses library to use (will be completed with /include and /lib)]))
    if test "x$with_ncurses" != "x"; then
		AC_MSG_NOTICE([using ncurses at $with_ncurses])
		LDFLAGS=${LDFLAGS}" -L$with_ncurses/lib"
		CPPFLAGS=${CPPFLAGS}" -I$with_ncurses/include"
    fi
	
    # Check for ncurses.h
    AC_CHECK_HEADER(ncurses.h, broken_ncurses=no, broken_ncurses=yes)

    # Check for functions wgetch in libncurses.a
    AC_CHECK_LIB(ncurses, wgetch, broken_ncurses=no, broken_ncurses=yes)

    if test "$broken_ncurses" == "yes"; then
		AC_MSG_NOTICE([ncurses not found. No line edition capabilities will be available.])
    else
		LIBS="-lncurses ${LIBS}"
		AC_DEFINE([HAVE_NCURSES], [], [Whether ncurses is available])
    fi
])

## UNISIM_CHECK_PTHREAD
## Checks if the pthread library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_PTHREAD], [
    # Check for pthread.h
    AC_CHECK_HEADER(pthread.h,, AC_MSG_ERROR([pthread.h not found.]))

    # Check for functions wgetch in libncurses.a
    AC_CHECK_LIB(pthread, pthread_create, broken_pthread=no, broken_pthread=yes)

    if test "$broken_pthread" == "yes"; then
		AC_MSG_NOTICE([POSIX thread library not found.])
    else
		LIBS="-lpthread ${LIBS}"
		AC_DEFINE([HAVE_PTHREAD], [], [Whether POSIX thread library is available])
    fi
])

## UNISIM_CHECK_UNISIM_LIB
## Checks if the unisim library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_UNISIM_LIB], [
    # Check if curses path has been overloaded
    AC_ARG_WITH(unisim_lib,
	AS_HELP_STRING([--with-unisim-lib=<path>], [unisim library to use (will be completed with /include and /lib)]))
    if test "x$with_unisim_lib" != "x"; then
	AC_MSG_NOTICE([using unisim library at $with_unisim_lib])
	LDFLAGS=${LDFLAGS}" -L$with_unisim_lib/lib"
	CPPFLAGS=${CPPFLAGS}" -I$with_unisim_lib/include"
    fi
	
    # Check for ncurses.h
    AC_CHECK_HEADER(unisim/kernel/kernel.hh,, AC_MSG_ERROR([service.hh not found. Please install the UNISIM library.]))
])

## UNISIM_CHECK_LIBEDIT
## Checks if the libedit library (Port of NetBSD alternative 'editline' to GNU GPL 'readline') is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBEDIT], [
    # Check if libedit path has been overloaded
    AC_ARG_WITH(libedit,
	AS_HELP_STRING([--with-libedit=<path>], [libedit library to use (will be completed with /include and /lib)]))
    if test "x$with_libedit" != "x"; then
	AC_MSG_NOTICE([using libedit at $with_libedit])
	LDFLAGS=${LDFLAGS}" -L$with_libedit/lib"
	CPPFLAGS=${CPPFLAGS}" -I$with_libedit/include"
    fi
	
    # Check for editline/readline.h
    AC_CHECK_HEADER(editline/readline.h, broken_libedit=no, broken_libedit=yes)

    # Check for functions readline and add_history in libedit
    AC_CHECK_LIB(edit, readline,
    AC_CHECK_LIB(edit, add_history, broken_libedit=no,
    broken_libedit=yes),
    broken_libedit=yes)
	
    if test "$broken_libedit" == "yes"; then
		AC_MSG_NOTICE([libedit (NetBSD editline port) not found. No line edition capabilities will be available.])
    else
		LIBS="-ledit ${LIBS}"
		AC_DEFINE([HAVE_LIBEDIT], [], [Whether libedit is available])
    fi
])

## UNISIM_CHECK_TERMCAP
## Checks if the termcap library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_TERMCAP], [
    # Check if termcap path has been overloaded
    AC_ARG_WITH(termcap,
	AS_HELP_STRING([--with-termcap=<path>], [termcap library to use (will be completed with /include and /lib)]))
    if test "x$with_termcap" != "x"; then
	AC_MSG_NOTICE([using termcap at $with_termcap])
	LDFLAGS=${LDFLAGS}" -L$with_termcap/lib"
	CPPFLAGS=${CPPFLAGS}" -I$with_termcap/include"
    fi
	
    # Check for termcap.h
    AC_CHECK_HEADER(termcap.h, broken_termcap=no, broken_termcap=yes)

    # Check for functions in termcap
    AC_CHECK_LIB(termcap, tgoto,
    AC_CHECK_LIB(termcap, tputs,
    AC_CHECK_LIB(termcap, tgetstr,
    AC_CHECK_LIB(termcap, tgetent,
    AC_CHECK_LIB(termcap, tgetflag,
    AC_CHECK_LIB(termcap, tgetnum, broken_termcap=no,
    broken_termcap=yes),
    broken_termcap=yes),
    broken_termcap=yes),
    broken_termcap=yes),
    broken_termcap=yes),
    broken_libedit=yes)
	
    if test "$broken_termcap" == "yes"; then
		AC_MSG_NOTICE([termcap not found. No line edition capabilities will be available.])
    else
		LIBS="-ltermcap ${LIBS}"
		AC_DEFINE([HAVE_TERMCAP], [], [Whether termcap is available])
    fi
])

## UNISIM_CHECK_SDL
## Checks if the SDL library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SDL], [
    # Check if SDL path has been overloaded
    AC_ARG_WITH(sdl,
	AS_HELP_STRING([--with-sdl=<path>], [sdl library to use (will be completed with /include and /lib)]))

    if test "x$with_sdl" != "x"; then
		AC_MSG_NOTICE([using SDL at $with_sdl])
		if test $host = $build; then
			# We are not crosscompiling so we can execute sdl-config on 'build' machine
			AC_CHECK_PROG(sdl_config_installed, sdl-config, yes, no, $with_sdl/bin)
			if test "x$sdl_config_installed" != "xyes"; then
					AC_MSG_ERROR([sdl-config not found. Please install SDL development library.])
			fi
			AC_MSG_NOTICE([sdl-config found])
			sdl_cflags="`$with_sdl/bin/sdl-config --cflags`"
			sdl_libs="`$with_sdl/bin/sdl-config --libs`"
			AC_MSG_NOTICE([sdl-config says compiler needs option ${sdl_cflags} ${sdl_libs} to compile and link with SDL])
			CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
			LDFLAGS=${LDFLAGS}" ${sdl_libs}"
		else
			AC_MSG_NOTICE([using SDL at $with_sdl])
			CPPFLAGS=${CPPFLAGS}" -I$with_sdl/include"
			LDFLAGS=${LDFLAGS}" -L$with_sdl/lib"
		fi
    else
		if test $host = $build; then
			# We are not crosscompiling so we can execute sdl-config on 'build' machine
			AC_CHECK_PROG(sdl_config_installed, sdl-config, yes, no)
			if test "x$sdl_config_installed" != "xyes"; then
					AC_MSG_ERROR([sdl-config not found. Please install SDL development library.])
			fi
			AC_MSG_NOTICE([sdl-config found])
			sdl_cflags="`sdl-config --cflags`"
			sdl_libs="`sdl-config --libs`"
			AC_MSG_NOTICE([sdl-config says compiler needs option ${sdl_cflags} ${sdl_libs} to compile and link with SDL])
			CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
			LDFLAGS=${LDFLAGS}" ${sdl_libs}"
		else
			sdl_cflags="-I/usr/include"
			sdl_libs="-L/usr/lib"
			AC_MSG_NOTICE([Trying with compiler option ${sdl_cflags} ${sdl_libs} to compile and link with SDL.])
			CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
			LDFLAGS=${LDFLAGS}" ${sdl_libs}"
		fi
	fi

    # Check for the main SDL header
    AC_CHECK_HEADER(SDL.h, broken_sdl=no, broken_sdl=yes)
	
    # Check for SDL functions
    AC_CHECK_LIB(SDL, SDL_WaitThread,
    AC_CHECK_LIB(SDL, SDL_FreeSurface,
    AC_CHECK_LIB(SDL, SDL_DestroyMutex,
    AC_CHECK_LIB(SDL, SDL_Quit,
    AC_CHECK_LIB(SDL, SDL_Init,
    AC_CHECK_LIB(SDL, SDL_CreateMutex,
    AC_CHECK_LIB(SDL, SDL_CreateThread,
    AC_CHECK_LIB(SDL, SDL_mutexP,
    AC_CHECK_LIB(SDL, SDL_mutexV,
    AC_CHECK_LIB(SDL, SDL_GetKeyName,
    AC_CHECK_LIB(SDL, SDL_Delay,
    AC_CHECK_LIB(SDL, SDL_PollEvent,
    AC_CHECK_LIB(SDL, SDL_SetVideoMode,
    AC_CHECK_LIB(SDL, SDL_WM_SetCaption,
    AC_CHECK_LIB(SDL, SDL_CreateRGBSurface,
    AC_CHECK_LIB(SDL, SDL_LockSurface,
    AC_CHECK_LIB(SDL, SDL_UnlockSurface,
    AC_CHECK_LIB(SDL, SDL_UpperBlit,
    AC_CHECK_LIB(SDL, SDL_UpdateRect,
    AC_CHECK_LIB(SDL, SDL_SaveBMP_RW, broken_sdl=no,
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes)

    if test "$broken_sdl" == "yes"; then
		AC_MSG_NOTICE([SDL not found. No video frame buffer or input devices will be available.])
    else
		LIBS="-lSDL ${LIBS}"
		AC_DEFINE([HAVE_SDL], [], [Whether SDL is available])
    fi
])

## UNISIM_CHECK_ZLIB
## Checks if the zlib library is installed
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_ZLIB], [
    # Check if zlib path has been overloaded
    AC_ARG_WITH(zlib,
	AS_HELP_STRING([--with-zlib=<path>], [zlib library to use (will be completed with /include and /lib)]))
    if test "x$with_zlib" != "x"; then
	AC_MSG_NOTICE([using zlib at $with_zlib])
	LDFLAGS=${LDFLAGS}" -L$with_zlib/lib"
	CPPFLAGS=${CPPFLAGS}" -I$with_zlib/include"
    fi
	
    # Check for zlib.h
    AC_CHECK_HEADER(zlib.h,, AC_MSG_ERROR([zlib.h not found. Please install the zlib development library (version >= 1.2.3).]))

    # Check for functions gzopen, gzclose and gzprintf in libz.a
    AC_CHECK_LIB(z, gzopen,
    AC_CHECK_LIB(z, gzdopen,
    AC_CHECK_LIB(z, gzread,
    AC_CHECK_LIB(z, gzrewind,
    AC_CHECK_LIB(z, deflateEnd,
    AC_CHECK_LIB(z, gzclose,
    AC_CHECK_LIB(z, gzwrite,
    AC_CHECK_LIB(z, deflate,
    AC_CHECK_LIB(z, crc32,
    AC_CHECK_LIB(z, deflateInit2_,
    AC_CHECK_LIB(z, inflateEnd,
    AC_CHECK_LIB(z, inflate,
    AC_CHECK_LIB(z, inflateInit2_,
    AC_CHECK_LIB(z, gzprintf, broken_zlib=no,
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes),
    broken_zlib=yes)

    if test "$broken_zlib" == "yes"; then
	AC_MSG_ERROR([installed zlib is broken.])
    else
	LIBS="-lz ${LIBS}"
    fi
])

## UNISIM_CHECK_LIBXML2
## Checks if the libxml2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBXML2], [
    # Check if libxml2 path has been overloaded
    AC_ARG_WITH(libxml2,
	AS_HELP_STRING([--with-libxml2=<path>], [libxml2 library to use (will be completed with /include and /lib)]))

    if test "x$with_libxml2" != "x"; then
		AC_MSG_NOTICE([using libxml2 at $with_libxml2])
		if test $host = $build; then
			# We are not crosscompiling so we can execute xml2-config on 'build' machine
			AC_CHECK_PROG(xml2_config_installed, xml2-config, yes, no, $with_libxml2/bin)
			if test "x$xml2_config_installed" != "xyes"; then
					AC_MSG_ERROR([xml2-config not found. Please install libxml2 development library.])
			fi
			AC_MSG_NOTICE([xml2-config found])
			libxml2_cflags="`$with_libxml2/bin/xml2-config --cflags`"
			libxml2_libs="`$with_libxml2/bin/xml2-config --libs`"
			AC_MSG_NOTICE([xml2-config says compiler needs option ${libxml2_cflags} ${libxml2_libs} to compile and link with libxml2])
			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
			LDFLAGS=${LDFLAGS}" ${libxml2_libs}"
		else
			CPPFLAGS=${CPPFLAGS}" -I$with_libxml2/include/libxml2"
			LDFLAGS=${LDFLAGS}" -L$with_libxml2/lib"
		fi
    else
		if test $host = $build; then
			# We are not crosscompiling so we can execute xml2-config on 'build' machine
			AC_CHECK_PROG(xml2_config_installed, xml2-config, yes, no)
			if test "x$xml2_config_installed" != "xyes"; then
					AC_MSG_ERROR([xml2-config not found. Please install libxml2 development library.])
			fi
			AC_MSG_NOTICE([xml2-config found])
			libxml2_cflags="`xml2-config --cflags`"
			libxml2_libs="`xml2-config --libs`"
			AC_MSG_NOTICE([xml2-config says compiler needs option ${libxml2_cflags} ${libxml2_libs} to compile and link with libxml2])
			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
			LDFLAGS=${LDFLAGS}" ${libxml2_libs}"
		else
			libxml2_cflags="-I/usr/include/libxml2"
			libxml2_libs="-L/usr/lib"
			AC_MSG_NOTICE([Trying with compiler option ${libxml2_cflags} ${libxml2_libs} to compile and link with libxml2.])
			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
			LDFLAGS=${LDFLAGS}" ${libxml2_libs}"
		fi
	fi
	
    # Check for some libxml2 headers
    AC_CHECK_HEADER(libxml/encoding.h, broken_incxml2=no, broken_incxml2=yes)
    AC_CHECK_HEADER(libxml/xmlwriter.h, , broken_incxml2=yes)
	
    if test "$broken_incxml2" == "yes"; then
		AC_MSG_ERROR([libxml2 includes not found (libxml/*.hh). Please use --with-libxml2=<path>])
    fi

    # Check for libxml2 functions
    AC_CHECK_LIB(xml2, xmlNewTextWriterFilename,
    AC_CHECK_LIB(xml2, xmlTextWriterSetIndent,
    AC_CHECK_LIB(xml2, xmlTextWriterStartDocument,
    AC_CHECK_LIB(xml2, xmlTextWriterStartElement, 
    AC_CHECK_LIB(xml2, xmlTextWriterEndElement, 
    AC_CHECK_LIB(xml2, xmlFreeTextWriter, 
    AC_CHECK_LIB(xml2, xmlTextWriterWriteAttribute, 
    AC_CHECK_LIB(xml2, xmlTextWriterWriteFormatString, broken_libxml2=no,
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes)
	
    if test "$broken_libxml2" == "yes"; then
		AC_MSG_ERROR([installed xml2 Library is broken.])
    else
		LIBS="-lxml2 ${LIBS}"
    fi
])

## UNISIM_CHECK_TLM20
## Checks if the OSCI TLM2.0 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_TLM20], [
	# Check if TLM2.0 path has been overloaded
	AC_ARG_WITH(tlm20,
	AS_HELP_STRING([--with-tlm20=<path>], [TLM2.0 library to use (will be completed with /include/tlm)]))
	if test "x$with_tlm20" != "x"; then
		AC_MSG_NOTICE([using TLM2.0 at $with_tlm20])
		CPPFLAGS=${CPPFLAGS}" -I$with_tlm20/include/tlm"
	fi

	# Check for tlm.h
	AC_CHECK_HEADER(tlm.h,, AC_MSG_ERROR([tlm.h not found for TLM2.0. Please install the TLM library (version 2.0). Use --with-tlm20=<path> to overload default includes search path.]))
])

## UNISIM_CHECK_SYSTEMC
## Checks if the libxml2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SYSTEMC], [
    # Mimics the behavior of SystemC configure to guess where libsystemc.a is installed (e.g. lib-linux)
    CXX_COMP=`basename $CXX`
    case "$host" in
	*powerpc*macosx* | *powerpc*darwin*)
	    SYSTEMC_TARGET_ARCH="macosx"
	;;
	*86*macosx* | *86*darwin*)
	    SYSTEMC_TARGET_ARCH="macosx-x86"
	;;
	sparc-sun-solaris*)
	    case "$CXX_COMP" in
	    CC)
		SYSTEMC_TARGET_ARCH="sparcOS5"
        	;;
	    c++ | g++)
		SYSTEMC_TARGET_ARCH="gccsparcOS5"
		;;
	    esac
	    ;;
	x86_64*linux*)
	    SYSTEMC_TARGET_ARCH="linux64"
    	    ;;
	*linux*)
    	    SYSTEMC_TARGET_ARCH="linux"
    	    ;;
        *cygwin*)
    	    SYSTEMC_TARGET_ARCH="cygwin"
    	    ;;
	*hpux11*)
    	    case "$CXX_COMP" in
        	aCC)
            	    SYSTEMC_TARGET_ARCH="hpux11"
            	    ;;
        	c++ | g++)
            	    SYSTEMC_TARGET_ARCH="gcchpux11"
            	    ;;
    	    esac
    	    ;;
	*mingw32*)
    	    SYSTEMC_TARGET_ARCH="mingw32"
    	    ;;
    esac

    # Check if SystemC path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-systemc=<path>], [systemc library to use (will be completed with /include and /lib-${SYSTEMC_TARGET_ARCH})]))
    if test "x$with_systemc" != "x"; then
	AC_MSG_NOTICE([using SystemC at $with_systemc])
	LDFLAGS=${LDFLAGS}" -L$with_systemc/lib-${SYSTEMC_TARGET_ARCH}"
	CPPFLAGS=${CPPFLAGS}" -I$with_systemc/include"
    fi
	CPPFLAGS=${CPPFLAGS}" -DSC_INCLUDE_DYNAMIC_PROCESSES"

    # Check for systemc.h
    AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.1). Use --with-systemc=<path> to overload default includes search path.]))

    # Check for function 'main' in libsystemc.a
    AC_CHECK_LIB(systemc,main,broken_systemc=no,broken_system=yes)

    if test "$broken_systemc" == "yes"; then
	AC_MSG_ERROR([installed SystemC is broken. Please install the SystemC library (version > 2.1). Use --with-systemc=<path> to overload default includes search path.])
    else
	LIBS="-lsystemc ${LIBS}"
    fi
])

## UNISIM_CHECK_CACTI
## Checks if the cacti library is installed
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_CACTI], [
    # Check if cacti path has been overloaded
    AC_ARG_WITH(cacti,
	AS_HELP_STRING([--with-cacti=<path>], [Cacti 4.2 library to use (will be completed with /include and /lib)]))
    if test "x$with_cacti" != "x"; then
	AC_MSG_NOTICE([using cacti at $with_cacti])
	CPPFLAGS=${CPPFLAGS}" -I$with_cacti/include"
    LDFLAGS=${LDFLAGS}" -L$with_cacti/lib"
    fi
	
    # Check for cacti4_2.hh
    AC_CHECK_HEADER(cacti4_2.hh, broken_cacti=no, broken_cacti=yes)

    if test "$broken_cacti" == "yes"; then
		AC_MSG_NOTICE([CACTI 4.2 not found. Cache power estimation will be unavailable.])
    else
        # Note: we can't check libcacti4_2 functions because it's a library of C++ classes with no C functions.
        LIBS="-lcacti4_2 ${LIBS}"
	fi
])

## UNISIM_CHECK_CXXABI
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_CXXABI], [
    # Check for execinfo.h
    AC_CHECK_HEADER(execinfo.h, broken_cxxabi=no, broken_cxxabi=yes)
    AC_CHECK_HEADER(cxxabi.h, , broken_cxxabi=yes)
    AC_CHECK_LIB(c, backtrace, , broken_cxxabi=yes)
    AC_CHECK_LIB(c, backtrace_symbols, , broken_cxxabi=yes)
    #AX_CXX_CHECK_LIB(stdc++, [abi::__cxa_demangle()], , broken_cxxabi=yes)

    if test "$broken_cxxabi" == "yes"; then
        AC_MSG_NOTICE([C++ ABI functions not found. No kernel debug will be available.])
    else
        AC_DEFINE([HAVE_CXXABI], [], [Whether C++ ABI functions are available])
        case "${CXX}" in
           *g++)
               LDFLAGS="${LDFLAGS} -rdynamic"
               ;;
           *)
               ;;
        esac
    fi
])

## UNISIM_CONFIG_SUBDIR
## 1st param: friendly name (like unisim-bla-bla)
## 2nd param: subdirectory name to enter if enabled (define COND_<subdirectory> automake macro)
#####################################################
AC_DEFUN([UNISIM_CONFIG_SUBDIR], [
AC_ARG_ENABLE($1,
	AS_HELP_STRING([--enable-$1], [enable compiling $1 (default)])
	AS_HELP_STRING([--disable-$1], [disable compiling $1]),
	[case "${enableval}" in
	yes) unisim_enabled=true ;;
	no) unisim_enabled=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-$1) ;;
	esac], [unisim_enabled=true])
	if test "x$3" != x; then
		supported_host1="$3"
		supported_host2="$4"
		supported_host3="$5"
		supported_host4="$6"
		supported_host5="$7"
		supported_host6="$8"
		supported_host7="$8"
		case $host in
			$supported_host1)
				;;
			*)
				case $host in
					$supported_host2)
						;;
					*)
						case $host in
							$supported_host3)
								;;
							*)
								case $host in
									$supported_host4)
										;;
									*)
										case $host in
											$supported_host5)
												;;
											*)
												case $host in
													$supported_host6)
														;;
													*)
														case $host in
															$supported_host7)
																;;
															*)
																unisim_enabled=false
																;;
														esac
														;;
												esac
												;;
										esac
										;;
								esac
								;;
						esac
						;;
				esac
				;;
		esac
	fi
	AM_CONDITIONAL(COND_$2, test $unisim_enabled == true)
	if test $unisim_enabled == true; then
		AC_CONFIG_SUBDIRS([$2])
	fi
])

## UNISIM_LINK
AC_DEFUN([UNISIM_LINK], [
	LIBS="-l$1 $LIBS"
])

## UNISIM_RELEASE
## Create the variable COND_release that the user can use 
##   in Makefile.am to decide what needs to be compile for release (--enable-release)
##   or just for development (--disable-release) (default)
## No parameters
#####################################################
AC_DEFUN([UNISIM_RELEASE], [
AC_ARG_ENABLE(release,
	AS_HELP_STRING([--enable-release], [build all the library versions])
	AS_HELP_STRING([--disable-release], [build the minimum number of library versions (default)]),
	[case "${enableval}" in
	yes) unisim_enabled_release=true ;;
	no) unisim_enabled_release=false ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-release) ;;
	esac], [unisim_enabled_release=false])
	AM_CONDITIONAL(COND_release, test x$unisim_enabled_release = xtrue)
])

# UNISIM_STATIC
# Create the variable COND_static that the user can use 
#   in Makefile.am to decide what needs to be compile for release (--enable-static)
#   or just for development (--disable-static) (default)
# No parameters
####################################################
AC_DEFUN([UNISIM_STATIC], [
AC_ARG_ENABLE(static-simulators,
	AS_HELP_STRING([--enable-static-simulators], [build statically linked simulators if possible])
	AS_HELP_STRING([--disable-static-simulators], [build dynamically linked simulators if possible (default)]),
	[case "${enableval}" in
	yes) unisim_enabled_static=yes ;;
	no) unisim_enabled_static=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-static-simulators) ;;
	esac], [unisim_enabled_static=no])
    if test "x$unisim_enabled_static" = "xyes"; then
		LDFLAGS="-static ${LDFLAGS}"
	fi
])

# UNISIM_LIBTOOL
# Add some additional argument to gcc command interpreted by libtool
# This macro should be called just before generating the Makefile
# No parameters
####################################################
AC_DEFUN([UNISIM_LIBTOOL], [
    if test "$unisim_enabled_static" = "yes"; then
		LDFLAGS="-all-static ${LDFLAGS}"
	fi
])
