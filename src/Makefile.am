##############################################################################
#
# 234Compositor - Image data merging library
#
# Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
##############################################################################


AUTOMAKE_OPTIONS = subdir-objects

#  lib_LTLIBRARIES = libPOLYmpi.la
#  libPOLYmpi_la_CXXFLAGS = @MPI_CFLAGS@ @TP_CFLAGS@ @REAL_OPT@ -I$(top_builddir)/include
#  libPOLYmpi_la_LDFLAGS = @LT_STATIC@
#  libPOLYmpi_la_SOURCES =

lib_LIBRARIES = lib234comp.a
lib234comp_a_CFLAGS = @MPI_CFLAGS@ -I$(top_builddir)/include
lib234comp_a_SOURCES = \
     234compositor.c \
     exchange.c \
     merge.c \
     misc.c


nobase_include_HEADERS = \
  $(top_builddir)/include/234compositor.h \
  $(top_builddir)/include/exchange.h \
  $(top_builddir)/include/merge.h \
  $(top_builddir)/include/misc.h \
  $(top_builddir)/include/234compVersion.h

EXTRA_DIST =
