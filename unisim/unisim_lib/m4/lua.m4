## UNISIM_CHECK_LUA
## Checks if the lua library is installed
## Takes one parameter: the main function name
#####################################################
AC_DEFUN([UNISIM_CHECK_LUA], [
    # Check for lua interpreter
    AX_PROG_LUA(5.1,,broken_lua=no, broken_lua=yes)
    
    if test "$broken_lua" == "yes"; then
		AC_MSG_NOTICE([LUA5.1 interpreter not found.])
	else
		# Check for lua headers
		AX_LUA_HEADERS(broken_lua=no, broken_lua=yes)

		if test "$broken_lua" == "yes"; then
			AC_MSG_NOTICE([LUA5.1 headers not found.])
		else
			#LIBS="-llua5.1 ${LIBS}"
			AC_DEFINE([HAVE_LUA], [], [Whether LUA library is available])
		fi
	fi
])
