##############################################################################
#
# 234Compositor - Image data merging library
#
# Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
##############################################################################

CC = mpifccpx 

# MPI_GATHER Version
CFLAGS	= -Kfast -Xg -std=c99 

# MPI_GATHERV Version
#CFLAGS	= -Kfast -Xg -std=c99 -D_GATHERV

# OpenMP
OMPFLAGS = -Kopenmp

LIBS = -lm

# =======================
INCLUDE_DIR = ./include
SOURCE_DIR  = ./src
LIB_DIR     = ./lib  
# =======================

OBJS_LIB = misc.o exchange.o merge.o 234compositor.o 
	  
AR      = ar
RANLIB  = ranlib
ARFLAG  = crsv
LIBFILE = lib234comp.a 
LIB_DIR  =  ../lib  

RM      = rm -f
MV	= mv  	  
CHDIR   = cd 

$(LIBFILE): $(OBJS_LIB) 
	$(AR) $(ARFLAG) $(LIBFILE) $(OBJS_LIB)
	$(RANLIB) $(LIBFILE)
	$(RM) $(OBJS_LIB) 
	$(MV) $(LIBFILE) $(LIB_DIR)	
	@echo "lib234comp.a created successfully."

%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) $(OMPFLAGS) -I$(INCLUDE_DIR) -c $<

all: $(LIBFILE)
	$(RM) *.o

clean:
	$(RM) $(LIB_DIR)/$(LIBFILE) *.o *~ core

