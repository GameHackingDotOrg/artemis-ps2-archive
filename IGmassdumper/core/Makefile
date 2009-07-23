
EE_BIN = core.elf

EE_INCS = -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I$(PS2SDK)/sbv/include
EE_LDFLAGS = -nostartfiles -Tlinkfile -L$(PS2SDK)/sbv/lib -L. -s
EE_LIBS = -lcdvd -lfileXio -ldebug -lpatches -lc -lkernel

EE_OBJS = core.o iopcontrol.o loadfile.o rpc.o
	
all:    
	$(MAKE) $(EE_BIN)

clean:
	rm -f *.elf *.irx *.o
	
	
include Makefile.pref
include Makefile.eeglobal
