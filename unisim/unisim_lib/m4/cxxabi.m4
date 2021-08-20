## UNISIM_CHECK_CXXABI
## Takes one parameter: the main function name
####################################################
AC_DEFUN([UNISIM_CHECK_CXXABI], [
    # Check for execinfo.h
    AC_CHECK_HEADER(execinfo.h, broken_cxxabi=no, broken_cxxabi=yes)
    AC_CHECK_HEADER(cxxabi.h, , broken_cxxabi=yes)
    UNISIM_CHECK_LIB(c, backtrace, , broken_cxxabi=yes)
    UNISIM_CHECK_LIB(c, backtrace_symbols, , broken_cxxabi=yes)

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
