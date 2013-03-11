#A literal copy paste of Berion's Artemis GUI
#All credit is his - Dnawrkshp

EE_BIN = LDv4_GUI.elf
EE_BIN_PACKED = LDv4_Dna_GUI.elf
EE_BIN_DIR = bin

EE_OBJS = main.o gui.o timer.o pad.o joker.o
EE_OBJS += font_neuropol.o
EE_OBJS += background.o logo.o logo_ghost.o
EE_OBJS += icon_start.o icon_stop.o icon_about.o icon_options.o icon_patches.o icon_mass.o icon_mc.o
EE_OBJS += desc_start.o desc_stop.o desc_about.o desc_options.o desc_patches.o desc_mc0.o desc_mc1.o desc_mass.o
EE_OBJS += icon_about_mini.o icon_options_mini.o icon_patches_mini.o
EE_OBJS += menu_bar.o sel_bar1.o sel_bar2.o
EE_OBJS += elf_loader.o
EE_OBJS += usbd.o usbhdfsd.o

EE_SRC = usbd.s usbhdfsd.s
EE_SRC += font_neuropol.s
EE_SRC += background.s logo.s logo_ghost.s
EE_SRC += icon_start.s icon_about.s icon_options.s icon_stop.s icon_patches.s icon_mass.s icon_mc.s
EE_SRC += desc_start.s desc_about.s desc_options.s desc_stop.s desc_patches.s desc_mc0.s desc_mc1.s desc_mass.s
EE_SRC += icon_about_mini.s icon_options_mini.s icon_patches_mini.s
EE_SRC += menu_bar.s sel_bar1.s sel_bar2.s
EE_SRC += elf_loader.s

EE_INCS = -I$(PS2DEV)/gsKit/include -I$(PS2SDK)/ports/include -I$(PS2SDK)/sbv/include
EE_LDFLAGS = -nostartfiles -L$(PS2DEV)/gsKit/lib -L$(PS2SDK)/ports/lib \
	-L$(PS2SDK)/sbv/lib -L. -s
EE_LIBS = -Xlinker --start-group -lpatches -lpad -lgskit -ldmakit -lm -lz -lpng -ldebug -Xlinker --end-group
	
all: $(EE_BIN)
	ps2_packer/ps2_packer $(EE_BIN) $(EE_BIN_PACKED)
	cp -f --remove-destination $(EE_BIN_PACKED) $(EE_BIN_DIR)/$(EE_BIN)
	make cleanup
 
run:	
	make clean
	make
	#EDIT TO YOUR IP ADDRESS
	ps2client -h 10.0.1.100 execee "host:$(EE_BIN_DIR)/$(EE_BIN)"
 
clean:
	$(MAKE) -C elf_loader clean		
	rm -f $(EE_OBJS) $(EE_SRC) *.elf *.ELF

cleanup:
	$(MAKE) -C elf_loader clean		
	rm -f $(EE_OBJS) $(EE_SRC) $(EE_BIN) $(EE_BIN_PACKED)
	
rebuild:
	$(MAKE) clean all
	
usbd.s:
	bin2s $(PS2SDK)/iop/irx/usbd.irx usbd.s usbd_irx
usbhdfsd.s:
	bin2s $(PS2DEV)/usbhdfsd/bin/usbhdfsd.irx usbhdfsd.s usb_mass_irx
font_neuropol.s:
	bin2s gui/font_neuropol.png font_neuropol.s font_neuropol
background.s:
	bin2s gui/background.png background.s background
logo.s:
	bin2s gui/CL-LiveDebug_logo.png logo.s logo
logo_ghost.s:
	bin2s gui/logo_ghost.png logo_ghost.s logo_ghost
icon_start.s:
	bin2s gui/icon_start.png icon_start.s icon_start
icon_stop.s:
	bin2s gui/icon_stop.png icon_stop.s icon_stop
icon_about.s:
	bin2s gui/icon_about.png icon_about.s icon_about
icon_options.s:
	bin2s gui/icon_options.png icon_options.s icon_options	
icon_patches.s:
	bin2s gui/icon_patches.png icon_patches.s icon_patches
icon_mc.s:
	bin2s gui/icon_mc.png icon_mc.s icon_mc
icon_mass.s:
	bin2s gui/icon_mass.png icon_mass.s icon_mass
desc_start.s:
	bin2s gui/desc_start.png desc_start.s desc_start
desc_stop.s:
	bin2s gui/desc_stop.png desc_stop.s desc_stop
desc_about.s:
	bin2s gui/desc_about.png desc_about.s desc_about
desc_options.s:
	bin2s gui/desc_options.png desc_options.s desc_options
desc_patches.s:
	bin2s gui/desc_patches.png desc_patches.s desc_patches
desc_mc0.s:
	bin2s gui/desc_mc0.png desc_mc0.s desc_mc0
desc_mc1.s:
	bin2s gui/desc_mc1.png desc_mc1.s desc_mc1
desc_mass.s:
	bin2s gui/desc_mass.png desc_mass.s desc_mass
icon_about_mini.s:
	bin2s gui/icon_about_mini.png icon_about_mini.s icon_about_mini	
icon_options_mini.s:
	bin2s gui/icon_options_mini.png icon_options_mini.s icon_options_mini	
icon_patches_mini.s:
	bin2s gui/icon_patches_mini.png icon_patches_mini.s icon_patches_mini
menu_bar.s:
	bin2s gui/menu_bar.png menu_bar.s menu_bar
sel_bar1.s:
	bin2s gui/sel_bar1.png sel_bar1.s sel_bar1
sel_bar2.s:
	bin2s gui/sel_bar2.png sel_bar2.s sel_bar2
		
elf_loader.s:
	$(MAKE) -C elf_loader
	bin2s elf_loader/elf_loader.elf elf_loader.s elf_loader

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
