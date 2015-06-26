#.SILENT:

#extension for object files
OXT = o

LIBDIR = ../lib

ifdef DEBUG
LIBNAME = $(LIBDIR)/libftkd.a
else
LIBNAME = $(LIBDIR)/libftk.a
endif

INC = ../INC

CFLAGS_D_LIB32 = -g -D_DEBUG
CFLAGS_R_LIB32 = -O2 -DNDEBUG
LFLAGS_D_LIB32 =

ifdef DEBUG
CFLAGS = $(CFLAGS_D_LIB32)
LFLAGS = $(LFLAGS_D_LIB32)
else
CFLAGS = $(CFLAGS_R_LIB32)
LFLAGS = $(LFLAGS_R_LIB32)
endif

CC = gcc

CPF =

.c.o:
	$(CC) $(CFLAGS) -c  $*.c $(CPF)

########################################################
## End Pretty much standard stuff
########################################################

all : $(LIBNAME)

clean :
	-rm *.o

#################################################

include ftkfiles.inc

$(LIBNAME) : $(3DSOBJS)
	ar rcuv $(LIBNAME) $(3DSOBJS)
