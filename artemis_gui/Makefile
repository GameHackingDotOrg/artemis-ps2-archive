
EE_BIN = ARTEMIS_GUI.ELF
EE_BIN_PACKED = packed_ARTEMIS_GUI.ELF
EE_BIN_DIR = bin

EE_OBJS = main.o gui.o timer.o pad.o usbd.o usbhdfsd.o
EE_OBJS += font_neuropol.o
EE_OBJS += background.o logo.o logo_ghost.o gshi_full.o
EE_OBJS += icon_start.o icon_cheats.o icon_about.o icon_options.o 
EE_OBJS += desc_start.o desc_cheats.o desc_about.o desc_options.o
EE_OBJS += icon_cheats_mini.o icon_about_mini.o icon_options_mini.o
EE_OBJS += menu_bar.o sel_bar1.o sel_bar2.o
EE_OBJS += elf_loader.o

EE_SRC = usbd.s usbhdfsd.s
EE_SRC += font_neuropol.s
EE_SRC += background.s logo.s logo_ghost.s gshi_full.s
EE_SRC += icon_start.s icon_cheats.s icon_about.s icon_options.s
EE_SRC += desc_start.s desc_cheats.s desc_about.s desc_options.s
EE_SRC += icon_cheats_mini.s icon_about_mini.s icon_options_mini.s
EE_SRC += menu_bar.s sel_bar1.s sel_bar2.s
EE_SRC += elf_loader.s

EE_INCS = -I$(PS2DEV)/gsKit/include -I$(PS2SDK)/ports/include -I$(PS2SDK)/sbv/include
EE_LDFLAGS = -nostartfiles -L$(PS2DEV)/gsKit/lib -L$(PS2SDK)/ports/lib \
	-L$(PS2SDK)/sbv/lib -L. -s
EE_LIBS = -Xlinker --start-group -lpatches -lpad -lgskit -ldmakit -lm -lz -lpng -ldebug -Xlinker --end-group
	
all: $(EE_BIN)
	ps2_packer/ps2_packer $(EE_BIN) $(EE_BIN_PACKED)
	cp -f --remove-destination $(EE_BIN_PACKED) $(EE_BIN_DIR)/$(EE_BIN)
 
clean:
	$(MAKE) -C elf_loader clean			
	rm -f $(EE_OBJS) $(EE_SRC) *.elf *.ELF

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
	bin2s gui/logo.png logo.s logo
logo_ghost.s:
	bin2s gui/logo_ghost.png logo_ghost.s logo_ghost
gshi_full.s:
	bin2s gui/gshi_full.png gshi_full.s gshi_full
icon_start.s:
	bin2s gui/icon_start.png icon_start.s icon_start
icon_cheats.s:
	bin2s gui/icon_cheats.png icon_cheats.s icon_cheats
icon_about.s:
	bin2s gui/icon_about.png icon_about.s icon_about
icon_options.s:
	bin2s gui/icon_options.png icon_options.s icon_options	
desc_start.s:
	bin2s gui/desc_start.png desc_start.s desc_start
desc_cheats.s:
	bin2s gui/desc_cheats.png desc_cheats.s desc_cheats
desc_about.s:
	bin2s gui/desc_about.png desc_about.s desc_about
desc_options.s:
	bin2s gui/desc_options.png desc_options.s desc_options
icon_cheats_mini.s:
	bin2s gui/icon_cheats_mini.png icon_cheats_mini.s icon_cheats_mini
icon_about_mini.s:
	bin2s gui/icon_about_mini.png icon_about_mini.s icon_about_mini	
icon_options_mini.s:
	bin2s gui/icon_options_mini.png icon_options_mini.s icon_options_mini	
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
