EE_BIN = CL-LiveDebug_v3_ucp.ELF
EE_OBJS = main.o Obj/cdvdinit_irx.o Obj/usbd.o Obj/usbhdfsd.o
EE_LDFLAGS = -s -Ttext 0x01800000
EE_LIBS = -lgraph -ldma -lc -lpacket -lpad

all: $(EE_BIN)
	ee-strip --strip-all $(EE_BIN)
	$(PS2SDK)/packer CL-LiveDebug_v3_ucp.ELF CL-LiveDebug_v3.ELF
	rm -f main.o CL-LiveDebug_v3_ucp.ELF

clean:
	rm -f *.elf *.o *.a $(EE_ELF) $(EE_ELF) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
