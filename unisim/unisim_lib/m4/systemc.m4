## UNISIM_CHECK_SYSTEMC
## Checks if the systemc library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SYSTEMC], [
	# Mimics the behavior of SystemC configure to guess where libsystemc.a is installed (e.g. lib-linux)
	CXX_COMP=`basename $CXX`
	case "$host" in
		x386-apple-* | i386-apple-*)
			SYSTEMC_TARGET_ARCH="macosx386"
			;;
		powerpc-apple-macosx*)
			SYSTEMC_TARGET_ARCH="macosx"
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
		*86*linux*)
			SYSTEMC_TARGET_ARCH="linux"
			;;
		*arm*linux*)
			SYSTEMC_TARGET_ARCH="linux-arm"
			;;
		powerpc*linux*)
			SYSTEMC_TARGET_ARCH="linux-powerpc"
			;;
		amd64*freebsd* | x86_64*freebsd*)
			SYSTEMC_TARGET_ARCH="freebsd64"
			;;
		*freebsd*)
			SYSTEMC_TARGET_ARCH="freebsd"
			;;
		*cygwin*)
			SYSTEMC_TARGET_ARCH="cygwin"
			;;
		amd64*mingw* | x86_64*mingw*)
			SYSTEMC_TARGET_ARCH="mingw64"
			;;
		*mingw*)
			SYSTEMC_TARGET_ARCH="mingw"
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
	esac

    # Check if SystemC path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-systemc=<path>], [Overrides search path to SystemC library]))
    if test -n "${with_systemc}"; then
		AC_MSG_NOTICE([using SystemC at $with_systemc])
		
		sep=':'

		case "${build}" in
			*mingw*)
				sep=';'    # separator on a windows build machine is ';', not ':'
				;;
		esac
		export PKG_CONFIG_PATH="${with_systemc}/lib-${SYSTEMC_TARGET_ARCH}/pkgconfig${sep}${with_systemc}/lib/pkgconfig${sep}${PKG_CONFIG_PATH}"
    fi
    
	PKG_CHECK_MODULES(SystemC, systemc, pkgconfig_found_systemc=yes, pkgconfig_found_systemc=no)

	if test "x$pkgconfig_found_systemc" = "xyes"; then
		AC_MSG_NOTICE([SystemC found])
		CPPFLAGS=${CPPFLAGS}" -DSC_INCLUDE_DYNAMIC_PROCESSES"
		CXXFLAGS="${CXXFLAGS} ${SystemC_CFLAGS}"
		LIBS="${LIBS} ${SystemC_LIBS}"
		UNISIM_CHECK_LIB_MAIN="sc_main"
	else
		CPPFLAGS=${CPPFLAGS}" -DSC_INCLUDE_DYNAMIC_PROCESSES"
		LDFLAGS="${LDFLAGS} -L$with_systemc/lib-${SYSTEMC_TARGET_ARCH}"
		CPPFLAGS="${CPPFLAGS} -I$with_systemc/include"
	
		# Check for systemc.h
		AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.3.0). Use --with-systemc=<path> to overload default search path.]))

		# Check for function 'sc_start' in libsystemc.a
		unisim_check_systemc_save_LIBS="${LIBS}"
		LIBS="-lsystemc ${LIBS}"
		AC_MSG_CHECKING([for sc_start in -lsystemc])
		AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <systemc.h>
extern "C"
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
__declspec(dllexport)
#endif
int sc_main(int argc, char **argv)
{
	sc_start();
	return 0;
}
extern "C"
int main(int argc, char *argv[])
{
	return sc_core::sc_elab_and_sim(argc, argv);
}]])],
		LIBS="${unisim_check_systemc_save_LIBS}"; AC_MSG_RESULT([yes]); [broken_systemc=no],
		LIBS="${unisim_check_systemc_save_LIBS}"; AC_MSG_RESULT([no]); [broken_systemc=yes])

		if test "x$broken_systemc" = "xyes"; then
			AC_MSG_ERROR([installed SystemC is broken. Please install the SystemC library (version > 2.3.0). Use --with-systemc=<path> to overload default search path.])
		else
			LIBS="-lsystemc ${LIBS}"
			UNISIM_CHECK_LIB_MAIN="sc_main"
		fi
	fi
])
