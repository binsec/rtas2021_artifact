bin_SCRIPTS = mkinitrd.sh
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = mkinitrd.source.sh

mkinitrd.sh: $(srcdir)/mkinitrd.source.sh
		rm -f mkinitrd.sh
		echo "#!${SH}" > mkinitrd.sh
		cat $(srcdir)/mkinitrd.source.sh >> mkinitrd.sh
		chmod ugo+x mkinitrd.sh
