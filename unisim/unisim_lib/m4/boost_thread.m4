## UNISIM_CHECK_BOOST_THREAD
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_THREAD], [
    # Check for boost thread headers
    AC_CHECK_HEADERS([boost/thread/thread.hpp boost/thread/mutex.hpp boost/thread/xtime.hpp],,\
	AC_MSG_ERROR([boost thread headers not found. Please install the boost thread development library. Use --with-boost=<path> to overload default includes search path.]))

	# Note: we can't check libboost_thread-mt functions from libboost_thread-mt because it's a library of C++ classes with no C functions.
	# TODO: Some distros still use the name libboost_thread, locate it.
	LIBS="-lboost_thread-mt ${LIBS}"
])
