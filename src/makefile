#----------------------------------------------------------------------------
# Current Author: Lazy Bastard, LBGSHI@gmail.com
# Original Author:	Tony Saveski, t_saveski@yahoo.com
#----------------------------------------------------------------------------
CC=ee-gcc
AS=ee-as
LD=ee-elf-ld
BMP2C=bmp2c

OBJ_DIR = obj
BMP_DIR = resources

CFLAGS = -Wall -W -EL -G0 -O0 -mips3 -nostdlib -DPS2_EE

BMP_SRC = BankGothicLtBT.c BankGothicMdBT.c BernardMTCondensed.c Consolas.c Gloucester.c NiagaraSolid.c OCRAExtended.c Proxy7.c Swis721BDCNOUL.c TempusSansITC.c TimesNewRoman.c VinerHandITC.c VinetaBT.c WideLatin.c

C_SRC = gs.c g2.c Artemis_Fonts.c $(BMP_SRC) fixed_tc.c
S_SRC = crt0.s ps2_asm.s dma_asm.s gs_asm.s

C_OBJ = $(addprefix $(OBJ_DIR)/, $(C_SRC:.c=.o))
S_OBJ = $(addprefix $(OBJ_DIR)/, $(S_SRC:.s=.o))

Artemis_Fonts.elf: $(C_OBJ) $(S_OBJ)
	@echo "-------------------------------------------------"
	$(CC) $(CFLAGS) -Tlinkfile -o Artemis_Fonts.elf $(C_OBJ) $(S_OBJ)

$(OBJ_DIR)/%.o: %.c
	@echo "-------------------------------------------------"
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.s
	@echo "-------------------------------------------------"
	$(CC) -xassembler-with-cpp -c $(CFLAGS) $< -o $@

%.c: $(BMP_DIR)/%.bmp
	@echo "-------------------------------------------------"
	$(BMP2C) $< $(*F) > $@

clean:
	rm -f $(C_OBJ) $(S_OBJ) *.elf $(BMP_SRC)

BankGothicLtBT.c: $(BMP_DIR)/BankGothicLtBT.bmp
BankGothicMdBT.c: $(BMP_DIR)/BankGothicMdBT.bmp
BernardMTCondensed.c: $(BMP_DIR)/BernardMTCondensed.bmp
Consolas.c: $(BMP_DIR)/Consolas.bmp
Gloucester.c: $(BMP_DIR)/Gloucester.bmp
NiagaraSolid.c: $(BMP_DIR)/NiagaraSolid.bmp
OCRAExtended.c: $(BMP_DIR)/OCRAExtended.bmp
Proxy7.c: $(BMP_DIR)/Proxy7.bmp
Swis721BDCNOUL.c: $(BMP_DIR)/Swis721BDCNOUL.bmp
TempusSansITC.c: $(BMP_DIR)/TempusSansITC.bmp
TimesNewRoman.c: $(BMP_DIR)/TimesNewRoman.bmp
VinerHandITC.c: $(BMP_DIR)/VinerHandITC.bmp
VinetaBT.c: $(BMP_DIR)/VinetaBT.bmp
WideLatin.c: $(BMP_DIR)/WideLatin.bmp