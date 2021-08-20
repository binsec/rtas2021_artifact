AC_DEFUN([UNISIM_CHECK_TIMES], [

	case $host in
		*mingw*)  # windows host
			;;
		*) # other hosts (darwin, linux and un*x)
			AC_CHECK_HEADER([sys/times.h],, AC_MSG_ERROR([sys/times.h is missing.]))
			;;
	esac

])
