#A literal copy paste of Berion's Artemis GUI
#All credit is his - Dnawrkshp

EE_BIN = LDv4_GUI.elf
EE_BIN_PACKED = LDv4_Dna_GUI.elf
EE_BIN_DIR = Release

IP_ADDR = 10.0.1.100
PACKER = "PS2 Packer/ps2_packer.exe"
SRC = Source
ElfL = ELF Loader
SND = Sound

EE_OBJS = $(SRC)/main.o $(SRC)/gui.o $(SRC)/timer.o $(SRC)/pad.o $(SRC)/joker.o $(SRC)/config.o $(SRC)/sound.o $(SRC)/textures.o
EE_OBJS += font_neuropol.o
EE_OBJS += background.o logo.o logo_ghost.o
EE_OBJS += icon_start.o icon_stop.o icon_about.o icon_options.o icon_patches.o icon_mass.o icon_mc.o
EE_OBJS += desc_start.o desc_stop.o desc_about.o desc_options.o desc_patches.o desc_mc0.o desc_mc1.o desc_mass.o
EE_OBJS += icon_about_mini.o icon_options_mini.o icon_patches_mini.o
EE_OBJS += menu_bar.o sel_bar1.o sel_bar2.o
EE_OBJS += usbd.o usbhdfsd.o freesd.o audsrv.o
#EE_OBJS += menu_trans.o start_game.o stop_disc.o
EE_OBJS += kb_frame.o key_sqr.o key_rect.o key_enter.o
EE_OBJS += yn_frame.o
EE_OBJS += elf_loader.o

EE_SRC = usbd.s usbhdfsd.s freesd.s audsrv.s
EE_SRC += font_neuropol.s
EE_SRC += background.s logo.s logo_ghost.s
EE_SRC += icon_start.s icon_about.s icon_options.s icon_stop.s icon_patches.s icon_mass.s icon_mc.s
EE_SRC += desc_start.s desc_about.s desc_options.s desc_stop.s desc_patches.s desc_mc0.s desc_mc1.s desc_mass.s
EE_SRC += icon_about_mini.s icon_options_mini.s icon_patches_mini.s
EE_SRC += menu_bar.s sel_bar1.s sel_bar2.s
#EE_SRC += menu_trans.s start_game.s stop_disc.s
EE_SRC += kb_frame.s key_sqr.s key_rect.s key_enter.s
EE_SRC += yn_frame.s
EE_SRC += elf_loader.s

EE_INCS = -I$(PS2DEV)/gsKit/include -I$(PS2SDK)/ports/include -I$(PS2SDK)/sbv/include
EE_LDFLAGS = -nostartfiles -L$(PS2DEV)/gsKit/lib -L$(PS2SDK)/ports/lib
EE_LIBS = -Xlinker --start-group -lconfig -lpatches -lpad -laudsrv -lgskit -ldmakit -lm -lz -lpng -ldebug -Xlinker --end-group
	
all: $(EE_BIN)
	$(PACKER) $(EE_BIN) $(EE_BIN_PACKED)
	cp -f --remove-destination $(EE_BIN_PACKED) $(EE_BIN_DIR)/$(EE_BIN)
	#cp -f --remove-destination $(EE_BIN) $(EE_BIN_DIR)/$(EE_BIN_PACKED)
	make cleanup
 
run:	
	ps2client -h $(IP_ADDR) reset
	make clean
	make
	#EDIT TO YOUR IP ADDRESS IN THE MAKEFILE
	ps2client -h $(IP_ADDR) execee "host:$(EE_BIN_DIR)/$(EE_BIN)"

exec:
	#EDIT TO YOUR IP ADDRESS IN THE MAKEFILE
	ps2client -h $(IP_ADDR) execee "host:$(EE_BIN_DIR)/$(EE_BIN)"
	
reset:
	ps2client -h $(IP_ADDR) reset
	
clean:
	$(MAKE) -C "$(ElfL)" clean		
	rm -f $(EE_OBJS) $(EE_SRC) *.elf *.ELF

cleanup:
	$(MAKE) -C "$(ElfL)" clean
	rm -f $(SRC)/main.o $(SRC)/gui.o $(SRC)/timer.o $(SRC)/pad.o $(SRC)/joker.o $(SRC)/config.o $(SRC)/sound.o $(SRC)/textures.o  $(EE_BIN) $(EE_BIN_PACKED)
	
rebuild:
	$(MAKE) clean all
	
%.adp : %.wav
	$(PS2SDK)/bin/adpenc $< $@
	
#IRX Modules
usbd.s:
	bin2s $(PS2SDK)/iop/irx/usbd.irx usbd.s usbd_irx
usbhdfsd.s:
	bin2s $(PS2DEV)/usbhdfsd/bin/usbhdfsd.irx usbhdfsd.s usb_mass_irx
freesd.s:
	bin2s $(PS2DEV)/ps2sdksrc/iop/sound/freesd/bin/freesd.irx freesd.s freesd_irx
audsrv.s:
	bin2s $(PS2DEV)/ps2sdk/iop/irx/audsrv.irx audsrv.s audsrv_irx
	
#ELF loader
elf_loader.s:
	$(MAKE) -C "$(ElfL)"
	bin2s "$(ElfL)/elf_loader.elf" elf_loader.s elf_loader

#Sounds
#menu_trans.s:
#	bin2s $(SND)/menu_trans.wav menu_trans.s menu_trans
#start_game.s:
#	bin2s $(SND)/start_game.wav start_game.s start_game
#stop_disc.s:
#	bin2s $(SND)/stop_disc.wav stop_disc.s stop_disc

#GUI images
font_neuropol.s:
	bin2s GUI/font_neuropol.png font_neuropol.s font_neuropol
background.s:
	bin2s GUI/background.png background.s background
logo.s:
	bin2s GUI/CL-LiveDebug_logo.png logo.s logo
logo_ghost.s:
	bin2s GUI/logo_ghost.png logo_ghost.s logo_ghost
icon_start.s:
	bin2s GUI/icon_start.png icon_start.s icon_start
icon_stop.s:
	bin2s GUI/icon_stop.png icon_stop.s icon_stop
icon_about.s:
	bin2s GUI/icon_about.png icon_about.s icon_about
icon_options.s:
	bin2s GUI/icon_options.png icon_options.s icon_options	
icon_patches.s:
	bin2s GUI/icon_patches.png icon_patches.s icon_patches
icon_mc.s:
	bin2s GUI/icon_mc.png icon_mc.s icon_mc
icon_mass.s:
	bin2s GUI/icon_mass.png icon_mass.s icon_mass
desc_start.s:
	bin2s GUI/desc_start.png desc_start.s desc_start
desc_stop.s:
	bin2s GUI/desc_stop.png desc_stop.s desc_stop
desc_about.s:
	bin2s GUI/desc_about.png desc_about.s desc_about
desc_options.s:
	bin2s GUI/desc_options.png desc_options.s desc_options
desc_patches.s:
	bin2s GUI/desc_patches.png desc_patches.s desc_patches
desc_mc0.s:
	bin2s GUI/desc_mc0.png desc_mc0.s desc_mc0
desc_mc1.s:
	bin2s GUI/desc_mc1.png desc_mc1.s desc_mc1
desc_mass.s:
	bin2s GUI/desc_mass.png desc_mass.s desc_mass
icon_about_mini.s:
	bin2s GUI/icon_about_mini.png icon_about_mini.s icon_about_mini	
icon_options_mini.s:
	bin2s GUI/icon_options_mini.png icon_options_mini.s icon_options_mini	
icon_patches_mini.s:
	bin2s GUI/icon_patches_mini.png icon_patches_mini.s icon_patches_mini
menu_bar.s:
	bin2s GUI/menu_bar.png menu_bar.s menu_bar
sel_bar1.s:
	bin2s GUI/sel_bar1.png sel_bar1.s sel_bar1
sel_bar2.s:
	bin2s GUI/sel_bar2.png sel_bar2.s sel_bar2
kb_frame.s:
	bin2s GUI/kb_frame.png kb_frame.s kb_frame
key_sqr.s:
	bin2s GUI/key_sqr.png key_sqr.s key_sqr
key_rect.s:
	bin2s GUI/key_rect.png key_rect.s key_rect
key_enter.s:
	bin2s GUI/key_enter.png key_enter.s key_enter
yn_frame.s:
	bin2s GUI/yn_frame.png yn_frame.s yn_frame
	
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
