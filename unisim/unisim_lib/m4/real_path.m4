AC_DEFUN([UNISIM_CHECK_REAL_PATH], [
	case ${host} in
		*mingw32*)  # win32 host
			AC_CHECK_HEADER([windows.h],, AC_MSG_ERROR([windows.h is missing.]))
			# Note: we can't check functions (GetFullPathNameA) because of the PASCAL calling convention. cdecl is mandatory for autoconf.
			;;
		*) # other hosts (preferably unixes)
			AC_CHECK_HEADERS([limits.h stdlib.h],, AC_MSG_ERROR([Some standard headers are missing.]))
			UNISIM_CHECK_LIB(c, realpath,, AC_MSG_ERROR([readlink support is missing.]))
			;;
	esac
])
