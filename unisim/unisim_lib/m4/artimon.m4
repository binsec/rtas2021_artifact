## UNISIM_CHECK_ARTIMON
#####################################################
AC_DEFUN([UNISIM_CHECK_ARTIMON], [
	# Check if ARTIMON path has been overloaded
	AC_ARG_WITH(artimon,
	AS_HELP_STRING([--with-artimon=<path>], [ARTIMON library to use (will be completed with /include)]))
	if test "x$with_artimon" != "x"; then
		AC_MSG_NOTICE([using ARTIMON at $with_artimon])
		LDFLAGS=${LDFLAGS}" -L$with_artimon/lib"
		CPPFLAGS=${CPPFLAGS}" -I$with_artimon/include"
	fi

	# Check for function 'artimon_refresh_time' in libARTIMON.a
	UNISIM_CHECK_LIB(ARTIMON,artimon_refresh_time,broken_artimon=no,broken_artimon=yes)

	if test "$broken_artimon" == "yes"; then
		AC_MSG_NOTICE([ARTIMON not found. Support for ARTIMON will be disabled.])
	else
		LIBS="-lARTIMON ${LIBS}"
		AC_DEFINE([HAVE_ARTIMON], [1], [Whether ARTIMON is available])
	fi

])
