AM_CXXFLAGS = -fPIC
INCLUDES = -I$(top_srcdir)
includedir = $(prefix)/include
sharedir = $(prefix)/share
include_HEADERS = cacti4_2.hh

lib_LIBRARIES = libcacti4_2.a
libcacti4_2_a_SOURCES = cacti4_2.cc
#lib_LTLIBRARIES = libcacti4_2.la
#libcacti4_2_la_SOURCES = cacti4_2.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)
	
