## UNISIM_CHECK_PYTHON
## Checks if the PYTHON development environment is available
#####################################################
AC_DEFUN([UNISIM_CHECK_PYTHON_DEV], [
    AX_PYTHON

    if test "x$PYTHON_INCLUDE_DIR" = "x";  then 
        AC_MSG_ERROR([Python development header not found (Python.h).])
    fi
    if test "x$PYTHON_LIB" = "x"; then
        AC_MSG_ERROR([Python development library not found.])
    fi
    
    LIBS="-l${PYTHON_LIB} ${LIBS}"
    CPPFLAGS="-I${PYTHON_INCLUDE_DIR} ${CPPFLAGS}"
])
