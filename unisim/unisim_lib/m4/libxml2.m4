## UNISIM_CHECK_LIBXML2
## Checks if the libxml2 library is installed
## Takes one parameter: the main function name
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBXML2], [
    # Check if libxml2 path has been overloaded
    AC_ARG_WITH(libxml2,
	AS_HELP_STRING([--with-libxml2=<path>], [libxml2 library to use (will be completed with /include and /lib)]))

    if test -n "$with_libxml2"; then
		AC_MSG_NOTICE([using libxml2 at $with_libxml2])
		if test "${host}" = "${build}"; then
			AC_CHECK_PROG(xml2_config_installed, xml2-config, yes, no, $with_libxml2/bin)
			if test "$xml2_config_installed" != "yes"; then
					AC_MSG_ERROR([xml2-config not found. Please install libxml2 development library.])
			fi
			AC_MSG_NOTICE([xml2-config found])
			libxml2_cflags="`$with_libxml2/bin/xml2-config --cflags`"
			libxml2_libs="`$with_libxml2/bin/xml2-config --libs`"
			AC_MSG_NOTICE([xml2-config says compiler needs option ${libxml2_cflags} ${libxml2_libs} to compile and link with libxml2])
		else
			# cross-compilation detected
			case "${host}" in
				*mingw32*)
					libxml2_cflags="-I${with_libxml2}/include/libxml2 -DLIBXML_STATIC"
					libxml2_libs="-L${with_libxml2}/lib -lxml2 -lz -lwsock32"
					;;
				*)
					libxml2_cflags="-I${with_libxml2}/include/libxml2"
					libxml2_libs="-L${with_libxml2}/lib -lxml2 -lz"
					;;
			esac
		fi
		CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
		LIBS=${LIBS}" ${libxml2_libs}"
    else
		AC_CHECK_PROG(xml2_config_installed, xml2-config, yes, no)
		if test "$xml2_config_installed" != "yes"; then
				AC_MSG_ERROR([xml2-config not found. Please install libxml2 development library.])
		fi
		AC_MSG_NOTICE([xml2-config found])
		libxml2_cflags="`xml2-config --cflags`"
		libxml2_libs="`xml2-config --libs`"
		AC_MSG_NOTICE([xml2-config says compiler needs option ${libxml2_cflags} ${libxml2_libs} to compile and link with libxml2])
		CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
		LIBS=${LIBS}" ${libxml2_libs}"
	fi
	
    # Check for some libxml2 headers
    AC_CHECK_HEADER(libxml/encoding.h, broken_incxml2=no, broken_incxml2=yes)
    AC_CHECK_HEADER(libxml/xmlwriter.h, , broken_incxml2=yes)
    AC_CHECK_HEADER(libxml/xmlreader.h, , broken_incxml2=yes)
	
    if test "$broken_incxml2" = "yes"; then
		AC_MSG_ERROR([libxml2 includes not found (libxml/*.hh). Please use --with-libxml2=<path>])
    fi

    # Check for libxml2 functions
	UNISIM_CHECK_LIB(xml2, xmlCharStrdup,
	UNISIM_CHECK_LIB(xml2, xmlCleanupParser,
	UNISIM_CHECK_LIB(xml2, xmlFree,
	UNISIM_CHECK_LIB(xml2, xmlFreeTextReader,
	UNISIM_CHECK_LIB(xml2, xmlFreeTextWriter,
	UNISIM_CHECK_LIB(xml2, xmlInitParser,
	UNISIM_CHECK_LIB(xml2, xmlNewTextWriterFilename,
	UNISIM_CHECK_LIB(xml2, xmlReaderForFile,
	UNISIM_CHECK_LIB(xml2, xmlStrEqual,
	UNISIM_CHECK_LIB(xml2, xmlTextReaderConstName,
	UNISIM_CHECK_LIB(xml2, xmlTextReaderConstValue,
	UNISIM_CHECK_LIB(xml2, xmlTextReaderGetAttribute,
	UNISIM_CHECK_LIB(xml2, xmlTextReaderNodeType,
	UNISIM_CHECK_LIB(xml2, xmlTextReaderRead,
	UNISIM_CHECK_LIB(xml2, xmlTextWriterEndDocument,
	UNISIM_CHECK_LIB(xml2, xmlTextWriterEndElement,
	UNISIM_CHECK_LIB(xml2, xmlTextWriterSetIndent,
	UNISIM_CHECK_LIB(xml2, xmlTextWriterSetIndentString,
	UNISIM_CHECK_LIB(xml2, xmlTextWriterStartDocument,
	UNISIM_CHECK_LIB(xml2, xmlTextWriterStartElement,
	UNISIM_CHECK_LIB(xml2, xmlTextWriterWriteAttribute,
	UNISIM_CHECK_LIB(xml2, xmlTextWriterWriteFormatString, broken_libxml2=no,
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes),
	broken_libxml2=yes)

    if test "$broken_libxml2" = "yes"; then
		AC_MSG_ERROR([installed xml2 Library is broken.])
    fi
])
