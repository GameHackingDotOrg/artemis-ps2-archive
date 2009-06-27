#  _____     ___ ____
#   ____|   |    ____|     
#  |     ___|   |____      
#                          
#  ------------------------------------------------------------------------

IOP_BIN = ntpbclient.irx
IOP_OBJS = ntpbclient.o imports.o

IOP_INCS += -I$(PS2SDK)/iop/include
IOP_CFLAGS += -Wall -fno-builtin $(DEBUG_FLAGS)
IOP_LDFLAGS += -s

all: $(IOP_BIN)

clean:
	-rm -f *.o *.irx

include $(PS2SDK)/Defs.make
include Rules.make

