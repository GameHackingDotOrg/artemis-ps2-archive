##
# Makefile for lsmod IOP module
##

IOP_BIN = lsmod.irx
IOP_OBJS = lsmod.o xprintf.o imports.o
IOP_LIBS =

IOP_INCS +=
IOP_CFLAGS += -O2 -Wall
IOP_ASFLAGS +=
IOP_LDFLAGS += -s


all: $(IOP_BIN)
	iop-size $(IOP_BIN)

clean:
	rm -f $(IOP_BIN) $(IOP_OBJS)

rebuild: clean all

run:
	ps2client -t 1 execiop host:$(IOP_BIN) host:modlist


# A rule to build imports.lst.
%.o : %.lst
	echo "#include \"irx_imports.h\"" > build-imports.c
	cat $< >> build-imports.c
	$(IOP_CC) $(IOP_CFLAGS) build-imports.c -o $@
	-rm -f build-imports.c

# A rule to build exports.tab.
%.o : %.tab
	echo "#include \"irx.h\"" > build-exports.c
	cat $< >> build-exports.c
	$(IOP_CC) $(IOP_CFLAGS) build-exports.c -o $@
	-rm -f build-exports.c

include $(PS2SDK)/Makefile.pref
include $(PS2SDK)/Makefile.iopglobal
