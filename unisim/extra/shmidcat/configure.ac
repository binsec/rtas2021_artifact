# Creating configure: aclocal -I m4 && autoconf --force && autoheader && libtoolize && automake -ac

AC_INIT([libshmidcat], [3.3.21])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_PATH_PROGS(SH, sh)
AC_PROG_CC
AC_PROG_INSTALL
AC_LIBTOOL_WIN32_DLL # must be *before* AC_PROG_LIBTOOL
AC_PROG_LIBTOOL
AC_ENABLE_SHARED
AC_PROG_LN_S

AC_LANG([C])

# -----------------------------------
    # Check if JDK path has been overloaded
AC_ARG_WITH(jdk,
	[AS_HELP_STRING([--with-jdk=<path>], [JDK home directory])])
    if test "x$with_jdk" != "x"; then
	AC_MSG_NOTICE([using JDK at $with_jdk])
case $host in
*mingw32*)  # win32 host
	CPPFLAGS=${CPPFLAGS}" -I$with_jdk/include -I$with_jdk/include/win32"
;;
*)
	CPPFLAGS=${CPPFLAGS}" -I$with_jdk/include -I$with_jdk/include/linux"
;;
esac

	JDK_PATH=$with_jdk
	AC_SUBST(JDK_PATH)
    fi

    # Check for jni.h
AC_CHECK_HEADER(jni.h,, AC_MSG_ERROR([jni.h not found. Please install the JDK. Use --with-jdk=<path> to overload default search path.]))

# --------------------------------------

AC_CHECK_HEADERS([stdio.h stdlib.h string.h sys/time.h sys/types.h unistd.h signal.h],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))


case $host in
*mingw32*)  # win32 host
	AC_CHECK_HEADER([windows.h],,AC_MSG_ERROR([windows.h is missing.]))
;;
*)
	AC_CHECK_HEADERS([sys/ipc.h sys/shm.h],,AC_MSG_ERROR([Some IPC System V headers are missing.]))
;;
esac

case $host in
	*mingw32*)  # win32 host
		if test "$enable_win32_dll" = "yes"; then
			LDFLAGS="-no-undefined -Wl,--add-stdcall-alias ${LDFLAGS}"
		fi
		;;
esac

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
