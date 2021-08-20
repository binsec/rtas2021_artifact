## UNISIM_CHECK_TVS
## Checks if the timed value streams library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_TVS], [
    # Check if TVS path has been overloaded
    AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-tvs=<path>], [Overrides search path to Timed-Value Streams library (TVS)]))
    if test -n "${with_tvs}"; then
		AC_MSG_NOTICE([using Timed-Value Streams library at $with_tvs])
		
		sep=':'

		case "${build}" in
			*mingw*)
				sep=';'    # separator on a windows build machine is ';', not ':'
				;;
		esac
		export PKG_CONFIG_PATH="${with_tvs}/lib/pkgconfig${sep}${with_tvs}/lib64/pkgconfig${sep}${PKG_CONFIG_PATH}"
    fi
    
	PKG_CHECK_MODULES(TVS, tvs, found_tvs=yes, found_tvs=no)

	if test "x$found_tvs" = "xyes"; then
		AC_MSG_NOTICE([Timed-Value Streams library found])
		CXXFLAGS="${CXXFLAGS} ${TVS_CFLAGS}"
		LIBS="${LIBS} ${TVS_LIBS}"
		AC_DEFINE([HAVE_TVS], [1], [Define to 1 if you have the Timed-Value Streams library (TVS)])
	else
		AC_MSG_NOTICE([Timed-Value Streams library (TVS) not found (optional). Please install the Timed-Value Streams library (version >= 0.10.0). Use --with-tvs=<path> to overload default search path.])
	fi
])
