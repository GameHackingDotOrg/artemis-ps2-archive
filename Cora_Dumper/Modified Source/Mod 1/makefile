EE_BIN  = Dumper.elf
EE_OBJS = main.o systemHook.s
EE_LIBS = -lpad -ldebug -lcdvd -lc

all: $(EE_BIN)

release: $(EE_BIN)
	 sjcrunch $(EE_BIN) packed.elf

clean:
	rm -f *.elf *.o

install:

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal