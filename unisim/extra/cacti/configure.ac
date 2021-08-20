AC_INIT([Cacti], [4.2])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_PATH_PROGS(SH, sh)
# AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_LN_S

AC_LANG([C++])

###### CHECK FOR STANDARD C HEADERS ###########################################

AC_CHECK_HEADERS([iostream stdio.h math.h stdlib.h string.h],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))

###### CHECK FOR STANDARD C++ HEADERS #########################################
AC_CHECK_HEADERS([iostream],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
