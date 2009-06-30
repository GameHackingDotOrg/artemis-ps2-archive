##
# Makefile for lsmod IOP module
##

IOP_BIN = lsmod.irx
IOP_OBJS = lsmod.o xprintf.o imports.o
IOP_LIBS =

IOP_INCS +=
IOP_CFLAGS += -Wall
IOP_ASFLAGS +=
IOP_LDFLAGS +=


all: $(IOP_BIN)
	iop-size $(IOP_BIN)

clean:
	rm -f $(IOP_BIN) $(IOP_OBJS)

rebuild: clean all

run:
	ps2client -t 1 execiop host:$(IOP_BIN) host:modlist


include $(PS2SDK)/Defs.make
include Rules.make
