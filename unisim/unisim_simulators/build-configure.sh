#!/bin/bash
file_list=`find . -name "configure.ac"`
here=`pwd`
m4_path=${here}/m4
for file in ${file_list}
do
	cd ${here}/`dirname $file`
	current_path=`pwd`
	has_to_build="false"
	if test -e configure.ac
	then
		if test -e configure;
		then
			if test ${m4_path}/acinclude.m4 -nt configure || test ${m4_path}/unisim.m4 -nt configure;
			then
				is_configure_ac_modified="true"
				has_to_build="true"
			fi
			if test configure.ac -nt configure
			then
				is_configure_ac_modified="true"
				has_to_build="true"
			fi
		else
			is_configure_missing="true"
			has_to_build="true"
		fi
	fi
	if test -e Makefile.am;
	then
		if test -e Makefile.in;
		then
			if test ${m4_path}/aminclude.am -nt Makefile.in || test ${m4_path}/unisim.am -nt Makefile.in;
			then
				is_makefile_am_modified="true"
				has_to_build="true"
			fi
			if test Makefile.am -nt Makefile.in;
			then
				is_makefile_am_modified="true"
				has_to_build="true"
			fi
		else
			is_makefile_missing="true"
			has_to_build="true"
		fi
	fi
	has_ac_prog_libtool=`grep AC_PROG_LIBTOOL configure.ac`
	if test "x$has_ac_prog_libtool" != "x";
	then
		if test ! -e config/ltmain.sh;
		then
			is_libtool_missing="true"
			has_to_build="true"
		fi
	fi
	if test "x$1" == "x--force";
	then 
		has_to_build="true"
		is_configure_missing="true"
		is_makefile_missing="true"
	fi
	message_to_display="$current_path ("
	if test "x$has_to_build" == "xtrue";
	then
		message_to_display="building:  $current_path";
		if test "x$is_configure_ac_modified" != "x";
		then
			message_to_display=$message_to_display" (configure.ac modified)"
		fi
		if test "x$is_configure_missing" != "x";
		then
			message_to_display=$message_to_display" (configure missing)"
		fi
		if test "x$is_makefile_am_modified" != "x";
		then
			message_to_display=$message_to_display" (Makefile.am modified)"
		fi
		if test "x$is_makefile_missing" != "x"
		then
			message_to_display=$message_to_display" (Makefile.in missing)"
		fi
		if test "x$is_libtool_missing" != "x"
		then
			message_to_display=$message_to_display" (config/ltmain.sh missing)"
		fi
		echo "$message_to_display"
		if test "x$has_ac_prog_libtool" != "x";
		then
			if [ `uname` == "Darwin" ]; then
				glibtoolize --force
			else
				libtoolize --force
			fi
		fi
		aclocal -I $m4_path
		autoconf --force
		has_ac_config_headers=`grep AC_CONFIG_HEADERS configure.ac`
		if test "x$has_ac_config_headers" != "x";
		then
			autoheader
		fi
		automake -ac
	else
		echo "skipping:  $current_path"
	fi
done
