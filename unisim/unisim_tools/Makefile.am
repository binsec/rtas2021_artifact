sharedir = $(prefix)/share/unisim_tools

SUBDIRS=
INCLUDE_SUBDIRS=

if COND_genisslib
SUBDIRS += genisslib
INCLUDE_SUBDIRS += genisslib
endif

if COND_mkinitrd
SUBDIRS += mkinitrd
INCLUDE_SUBDIRS += mkinitrd
endif

dist_share_DATA = COPYING

EXTRA_DIST = \
m4/acinclude.m4 \
m4/aminclude.am \
m4/lexer.m4 \
m4/parser_gen.m4 \
m4/unisim.am \
m4/unisim.m4

uninstall-hook:
	(rmdir $(sharedir) || true)


include m4/aminclude.am
