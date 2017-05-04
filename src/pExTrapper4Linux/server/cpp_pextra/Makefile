# //////////////////////////////////////////////////////////////////////////////
# // Copyright 2005-2008 Plustar Corporation. All rights reserved. プラスター

# Makefile

CC=g++
CFLAGS+= 
CPPFLAGS+= $(CFLAGS)
LDFLAGS+=

TARG0=pextra
OBJ0=base64.o iftools.o pextra.o

TARG1=bcast
OBJ1=iftools.o bcast.o

#

all:
	make $(TARG0) $(TARG1)
	@echo "OK!";

re:
	make clean all

clean:
	rm -f *~ *.bak
	rm -f $(TARG0) $(OBJ0)
	rm -f $(TARG1) $(OBJ1)

release:
	cp pextra pextra.release

$(TARG0): $(OBJ0) -lpthread

$(TARG1): $(OBJ1)
