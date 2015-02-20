##############################################################################
#
# 234Compositor - Image data merging library
#
# Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
##############################################################################


SUBDIRS = @MAKE_SUB_DIRS@

bin_SCRIPTS    = 234comp-config
CLEANFILES     = $(bin_SCRIPTS)
EXTRA_DIST     = 234comp-config.in LICENSE doc/doxygen/Doxyfile \
                 Makefile_hand Makefile_hand.K Makefile_hand.shared
dist_data_DATA = AUTHORS COPYING ChangeLog README LICENSE sample

#ACLOCAL_AMFLAGS = -I m4 // SO

DISTCLEANFILES = *.tar.gz


# to check libtool is upto date // SO
#LIBTOOL_DEPS = @LIBTOOL_DEPS@
#libtool:$(LIBTOOL_DEPS)
#	$(SHELL) ./config.status --recheck

#clean-local: // SO
#	find ./ -name *.exe | xargs rm -f
#	find ./ -name *.lib | xargs rm -f


# clean-local:
	find ./ -name *.obj | xargs rm -f
	find ./ -name *.exe | xargs rm -f
	find ./ -name *.lib | xargs rm -f

distclean-local: clean-local
	rm -f ./bin/*
	rm -f ./lib/*
