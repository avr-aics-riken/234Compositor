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


noinst_PROGRAMS = test_234byte_mandel test_234float_mandel



DISTCLEANFILES=*~ test_234byte_mandel test_234float_mandel
CLEANFILES=data/*.log

EXTRA_DIST= \
   compile.sh \
   test_234byte_mandel.c \
   test_234float_mandel.c


test_234byte_mandel_SOURCES =  test_234byte_mandel.c
test_234byte_mandel_CFLAGS  = -I$(top_builddir)/include @MPI_CFLAGS@

test_234float_mandel_SOURCES = test_234float_mandel.c
test_234float_mandel_CFLAGS  = -I$(top_builddir)/include @MPI_CFLAGS@



# //SO
# test_LDADD = \
#     -L$(top_builddir)/src -l234comp \
#     @MPI_LDFLAGS@ \
#     @MPI_LIBS@ \
#      -lstdc++

test_234byte_mandel_LDADD = \
     -L$(top_builddir)/src -l234comp \
     @MPI_LDFLAGS@ \
     @MPI_LIBS@

test_234float_mandel_LDADD = \
     -L$(top_builddir)/src -l234comp \
     @MPI_LDFLAGS@ \
     @MPI_LIBS@


dist_noinst_DATA= GLUT

.PHONY:$(dist_noinst_DATA)


