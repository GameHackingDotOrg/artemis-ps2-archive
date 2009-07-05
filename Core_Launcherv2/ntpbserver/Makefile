
EE_BIN_PACKED = core_launcher.elf
EE_BIN = core_launcher_unpacked.elf
EE_OBJS = core_launcher.o core.o memdisk.o eesync.o ntpbserver.o ps2dev9.o ps2ip.o ps2smap.o
EE_LIBS = -lpad -lcdvd -lpatches -ldebug -lc -lkernel
EE_LDFLAGS = -s

all:
	$(MAKE) $(EE_BIN)
	ps2_packer/ps2_packer $(EE_BIN) $(EE_BIN_PACKED)
	$(MAKE) -C core clean
	$(MAKE) -C irx/memdisk clean
	$(MAKE) -C irx/eesync clean	
	$(MAKE) -C irx/ntpbserver clean	
	$(MAKE) -C irx/dev9 clean					
	rm -f bin/$(EE_BIN_PACKED)		
	cp $(EE_BIN_PACKED) bin/$(EE_BIN_PACKED)
	rm -f *.elf *.o *.a *.s

clean:
	$(MAKE) -C core clean
	$(MAKE) -C irx/memdisk clean
	$(MAKE) -C irx/eesync clean	
	$(MAKE) -C irx/ntpbserver clean	
	$(MAKE) -C irx/dev9 clean					
	rm -f *.elf *.o *.a *.s
		
memdisk.s:
	$(MAKE) -C irx/memdisk
	bin2s irx/memdisk/memdisk.irx memdisk.s memdisk_irx

eesync.s:
	$(MAKE) -C irx/eesync
	bin2s irx/eesync/eesync.irx eesync.s eesync_irx

ntpbserver.s:
	$(MAKE) -C irx/ntpbserver
	bin2s irx/ntpbserver/ntpbserver.irx ntpbserver.s ntpbserver_irx

ps2dev9.s:
	$(MAKE) -C irx/dev9
	bin2s irx/dev9/bin/ps2dev9.irx ps2dev9.s ps2dev9_irx	
	
ps2ip.s:
	bin2s $(PS2SDK)/iop/irx/ps2ip.irx ps2ip.s ps2ip_irx
	
ps2smap.s:
	bin2s $(PS2DEV)/ps2eth/smap/ps2smap.irx ps2smap.s ps2smap_irx
					
core.s:
	$(MAKE) -C core
	bin2s core/core.elf core.s core_elf
					
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
