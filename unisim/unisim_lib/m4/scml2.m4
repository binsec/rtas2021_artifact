## UNISIM_CHECK_SCML2
## Checks if the SCML2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_SCML2], [
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
	# Check if SCML2 path has been overloaded
	AC_ARG_WITH(scml2,
	AS_HELP_STRING([--with-scml2=<path>], [SCML2 library to use (will be completed with /include)]))
	if test "x$with_scml2" != "x"; then
		AC_MSG_NOTICE([using SCML2 at $with_scml2])
		CPPFLAGS=${CPPFLAGS}" -I$with_scml2/include"
		LDFLAGS="${LDFLAGS} -L$with_scml2/lib-${SYSTEMC_TARGET_ARCH}"
		LIBS="-lscml2 -lscml_clock -lscml2_logging -lModelingObjects ${LIBS}"
	fi

	# Check for tlm.h
	AC_CHECK_HEADER(scml2.h,, AC_MSG_ERROR([scml2.h not found for SCML2. Please install the SCML2 library (version 2.2). Use --with-scml2=<path> to overload default includes search path.]))
])
