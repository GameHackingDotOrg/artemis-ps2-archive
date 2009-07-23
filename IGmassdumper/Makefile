
EE_BIN_PACKED = IGmassdumper.elf
EE_BIN = IGmassdumper_unpacked.elf
EE_OBJS = IGmassdumper.o core.o memdisk.o eesync.o usbd.o usbhdfsd.o usbserv.o
EE_LIBS = -lpad -lcdvd -lpatches -ldebug -lc -lkernel
EE_LDFLAGS = -s

all:
	$(MAKE) $(EE_BIN)
	ps2_packer/ps2_packer $(EE_BIN) $(EE_BIN_PACKED)
	$(MAKE) -C core clean
	$(MAKE) -C irx/memdisk clean
	$(MAKE) -C irx/eesync clean	
	$(MAKE) -C irx/usbserv clean		
	rm -f bin/$(EE_BIN_PACKED)		
	cp $(EE_BIN_PACKED) bin/$(EE_BIN_PACKED)
	rm -f *.elf *.o *.a *.s

clean:
	$(MAKE) -C core clean
	$(MAKE) -C irx/memdisk clean
	$(MAKE) -C irx/eesync clean	
	$(MAKE) -C irx/usbserv clean		
	rm -f *.elf *.o *.a *.s
		
memdisk.s:
	$(MAKE) -C irx/memdisk
	bin2s irx/memdisk/memdisk.irx memdisk.s memdisk_irx

eesync.s:
	$(MAKE) -C irx/eesync
	bin2s irx/eesync/eesync.irx eesync.s eesync_irx

usbd.s:
	bin2s $(PS2SDK)/iop/irx/usbd.irx usbd.s usbd_irx

usbhdfsd.s:
	bin2s $(PS2DEV)/usbhdfsd/bin/usbhdfsd.irx usbhdfsd.s usb_mass_irx
			
usbserv.s:
	$(MAKE) -C irx/usbserv
	bin2s irx/usbserv/usbserv.irx usbserv.s usbserv_irx
				
core.s:
	$(MAKE) -C core
	bin2s core/core.elf core.s core_elf
					
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
