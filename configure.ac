##############################################################################
#
# 234Compositor - Image data merging library
#
# Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
##############################################################################
#
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])
AC_INIT([234Compositor], [0.9.1], [jorji@riken.jp], [lib234Compositor])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([include/234compositor.h])
AC_CONFIG_HEADERS([config.h])

# add to switch libtool
#AC_CONFIG_MACRO_DIR([m4]) // SO

#
# revision No.
#
CMPSTR_REVISION="20150223_2200"
AC_SUBST(CMPSTR_REVISION)


CMPSTR_CC="$CC"
AC_SUBST(CMPSTR_CC)


enable_mpi="none"

# if a wrapper compiler is used, "enable_mpi" is empty, but parallel.
# wrappers like mpicxx, mpicc, mpiCC, mpic++, mpiFCCpx are taking into consideration, if not add in case.
# CC is for Hopper

case "$CMPSTR_CC" in
mpicc)    enable_mpi="mpi" ;;
mpiCC)    enable_mpi="mpi" ;;
mpifccpx) enable_mpi="mpi" ;;
CC)       enable_mpi="mpi" ;;
esac


#
# MPI environment
#
AC_SUBST(MPI_DIR)
AC_SUBST(MPI_CFLAGS)
AC_SUBST(MPI_LDFLAGS)
AC_SUBST(MPI_LIBS)
#AC_SUBST(_MPI_INCLUDE)



# mpich
AC_ARG_WITH(mpich, [AC_HELP_STRING([--with-mpich=dir],[Specify MPICH install directory])], , with_mpich=none)
if test x"$with_mpich" != x"none" ; then
MPI_DIR=$with_mpich;
MPI_CFLAGS=-I$MPI_DIR/include
MPI_LDFLAGS=-L$MPI_DIR/lib
MPI_LIBS=-lmpich
enable_mpi="mpich"
fi


# openmpi
if test x"$with_mpich" = x"none" ; then
AC_ARG_WITH( ompi, [AC_HELP_STRING([--with-ompi=dir],[Specify OpenMPI install directory])], , with_ompi=none)
if test x"$with_ompi" != x"none" ; then
MPI_DIR=$with_ompi;
MPI_CFLAGS=-I$MPI_DIR/include
MPI_LDFLAGS=-L$MPI_DIR/lib
MPI_LIBS=-lmpi
enable_mpi="ompi"
fi
fi

if test x"$enable_mpi" = x"none" ; then
echo "  Error: Neither MPICH nor OpenMPI library is not specified."
echo "           If compilation error occurs about MPI, specify --with-mpich or --with-ompi option."
fi



#
# default prefix setting
#
AC_PREFIX_DEFAULT([/usr/local/lib234Compositor])
if test x"$prefix" = x"NONE" ; then
CMPSTR_INST_DIR=/usr/local/lib234Compositor
else
CMPSTR_INST_DIR="$prefix"
fi



#
# example
#
AC_ARG_WITH(example, [AC_HELP_STRING([--with-example=(no|yes)],[make example [no]])], , with_example=no)



#
# enable/disable shared library // SO
#
#dnl AC_ARG_ENABLE(shared, [AC_HELP_STRING([--enable-shared=(no|yes)],[make shared library [yes]])], , enable_shared=no)

#dnl if test "$enable_shared" != "yes" ; then
#dnl LT_STATIC="-static"
#dnl else
#dnl LT_STATIC=""
#dnl fi
#dnl AC_SUBST(LT_STATIC)


#
# 234Compositor special flags
#

CMPSTR_LIBS="-l234comp"
AC_SUBST(CMPSTR_LIBS)

CMPSTR_CFLAGS="-I$CMPSTR_INST_DIR/include"
AC_SUBST(CMPSTR_CFLAGS)

CMPSTR_LDFLAGS="-L$CMPSTR_INST_DIR/lib"
AC_SUBST(CMPSTR_LDFLAGS)



#
# MAKE_SUB_DIRS
#

AC_SUBST(MAKE_SUB_DIRS)
MAKE_SUB_DIRS="src doc"
if test x"$with_example" = x"yes" ; then
MAKE_SUB_DIRS="$MAKE_SUB_DIRS"" sample"
fi



# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB


# add to switch libtool // SO
#AC_PROG_LIBTOOL
#AM_PROG_LIBTOOL
#AC_SUBST(LIBTOOL_DEPS)



# Checks for libraries.



# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_RESTRICT

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor memset pow])


AC_CONFIG_FILES([Makefile \
                 doc/Makefile \
                 src/Makefile \
                 sample/Makefile \
                 include/234compVersion.h \
                 234comp-config ])
AC_OUTPUT

chmod +x ./234comp-config
