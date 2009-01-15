##
# Copyright (C) 2008 misfire <misfire@xploderfreax.de>
#
#  - Added -O2 and -s flags.  Removed -g.
#  - Added rebuild and install targets.
#
# Copyright (C) 2005 Mega Man
##

PROGRAM = irxtool
CFLAGS	= -Wall -O2
LDFLAGS = -s

all: $(PROGRAM)

clean:
	rm -f $(PROGRAM)
	rm -f $(PROGRAM).o

rebuild: clean all

install: all
	cp -f $(PROGRAM) $(PS2DEV)/bin/

import.db:
	#./generatedb.pl $(PS2SDKSRC)/iop/kernel/include/*.h >import.db
	find $(PS2SDKSRC)/iop/ -iname "*.h" | xargs ./generatedb.pl >import.db

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) -c -o $@ $^

$(PROGRAM): $(PROGRAM).o
	$(CC) $(LDFLAGS) -o $@ $^

