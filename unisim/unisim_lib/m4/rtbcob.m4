## UNISIM_CHECK_RTBCOB
#####################################################
AC_DEFUN([UNISIM_CHECK_RTBCOB], [
	# Check if RTBuilder Cob path has been overloaded
	AC_ARG_WITH(rtbcob,
	AS_HELP_STRING([--with-rtbcob=<path>], [RTBuilder Cob library to use (will be completed with /include)]))
	if test "x$with_rtbcob" != "x"; then
		AC_MSG_NOTICE([using RTBuilder Cob at $with_rtbcob])
		LDFLAGS=${LDFLAGS}" -L$with_rtbcob/rtbuilderLibrary/clib/msw"
		CPPFLAGS=${CPPFLAGS}" -I$with_rtbcob/include"
	fi

	# Check for function 'init_network' in libcob.a
	UNISIM_CHECK_LIB(cob,init_network,broken_rtbcob=no,broken_rtbcob=yes)

	if test "$broken_rtbcob" == "yes"; then
		AC_MSG_NOTICE([RTBuilder COB not found. Support for RTBuilder COB will be disabled.])
	else
		LIBS="-lcob ${LIBS}"
		AC_DEFINE([HAVE_RTBCOB], [], [Whether RTBuilder COB is available])
	fi

])
