##############################################################################
#
# 234Compositor - Image data merging library
#
# Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
##############################################################################

####
#### if you compile these tes programs on K/FX, remove -lstdc++ from LDADD lines
####


noinst_PROGRAMS = test_234byte test_234float



DISTCLEANFILES=*~ test_234byte test_234float
CLEANFILES=data/*.log

EXTRA_DIST= \
   compile.sh \
   test_234byte.c \
   test_234float.c


test_234byte_SOURCES =  test_234byte.c
test_234byte_CFLAGS  = -I$(top_builddir)/include @MPI_CFLAGS@

test_234float_SOURCES = test_234float.c
test_234float_CFLAGS  = -I$(top_builddir)/include @MPI_CFLAGS@



# //SO
# test_LDADD = \
#     -L$(top_builddir)/src/.libs -l234comp \
#     @MPI_LDFLAGS@ \
#     @MPI_LIBS@ \
#      -lstdc++

test_234byte_LDADD = \
     -L$(top_builddir)/src -l234comp \
     @MPI_LDFLAGS@ \
     @MPI_LIBS@

test_234float_LDADD = \
-L$(top_builddir)/src -l234comp \
@MPI_LDFLAGS@ \
@MPI_LIBS@


dist_noinst_DATA= input_images

.PHONY:$(dist_noinst_DATA)


