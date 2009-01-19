#
# ====================================================================================
#     _____               __     __                _____                  _       
#   / ____|              \ \   / /               / ____|                (_)      
#  | |     __ _ _ __ _____\ \_/ /__  _   _ _____| (___  _   _ _ ____   _____   _____ 
# | |    / _` | '_ \______\   / _ \| | | |______\___ \| | | | '__\ \ / / \ \ / / _ \
#| |___| (_| | | | |      | | (_) | |_| |      ____) | |_| | |   \ V /| |\ V /  __/
#\_____\__,_|_| |_|      |_|\___/ \__,_|     |_____/ \__,_|_|    \_/ |_| \_/ \___|
# ====================================================================================

ELF_V	= ScreenPrint1Compressed
EE_BIN  = screenPrint1.elf
EE_OBJS = main.o codeloader.o ps2_asm.o g2.o
EE_LIBS = -lpad -ldebug -lcdvd -lc


all: 	$(EE_BIN)
	 #rm -f *.o
	
release: $(EE_BIN)
	 sjcrunch $(EE_BIN) $(ELF_V).elf
	 rm -f *.o

strip:
	 ee-strip -F elf32-littlemips $(EE_BIN)
	 
	
clean:
	rm -f *.elf *.o

install:

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
