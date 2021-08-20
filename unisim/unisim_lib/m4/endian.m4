AC_DEFUN([UNISIM_CHECK_ENDIAN], [

	case $host in
		*darwin*)  # darwin host
			AC_CHECK_HEADER([sys/types.h],, AC_MSG_ERROR([sys/types.h is missing.]))
			;;

		*mingw32*) # win32 host
			;;

		
		*) # other hosts (linux and un*x)
			AC_CHECK_HEADER([endian.h],, AC_MSG_ERROR([endian.h is missing.]))
			;;
	esac

])
