##
# Makefile for dump2mass
##

EE_INCS +=
EE_CFLAGS += -D_DEBUG #-D_IOPRESET
EE_CXXFLAGS +=
EE_LDFLAGS += -s
EE_ASFLAGS +=

EE_BIN = dump2mass.elf
EE_OBJS = crt0.o main.o usbd_irx.o usb_mass_irx.o
EE_LIBS = -ldebug -lpatches

all: dump2mass

clean:
	@rm -f $(EE_BIN) $(EE_OBJS) usbd_irx.s usb_mass_irx.s

rebuild: clean all

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

dump2mass: $(EE_OBJS)
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(EE_LDFLAGS) \
	-o $(EE_BIN) $(EE_OBJS) $(EE_LIBS)

usbd_irx.s:
	bin2s $(PS2SDK)/iop/irx/usbd.irx usbd_irx.s usbd_irx

usb_mass_irx.s:
	bin2s usb_mass.irx usb_mass_irx.s usb_mass_irx

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
