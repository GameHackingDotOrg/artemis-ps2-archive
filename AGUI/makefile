EE_INCS := -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I. $(EE_INCS) -I$(PS2SDK)/ports -I$(PS2SDK)/ports/include
EE_LIBS := -lkernel -lc -lpadx -lc -lkernel
EE_LDFLAGS := -L$(PS2SDK)/ee/lib $(EE_LDFLAGS)
CFLAGS = -EL -G0 -mips3 -nostdlib -DPS2_EE

C_SRC = gs.c g2.c agui.c ssplash.c sMainS.c sMainO.c sCodeA.c sOptions1.c sOptions2.c sOptions3.c sStartC.c sStartS.c
S_SRC = crt0.s ps2_asm.s dma_asm.s gs_asm.s

C_OBJ = $(addprefix obj/, $(C_SRC:.c=.o))
S_OBJ = $(addprefix obj/, $(S_SRC:.s=.o))




atemp.elf: $(C_OBJ) $(S_OBJ)
	@echo "-------------------------------------------------"
	ee-gcc -Tlinkfile $(CFLAGS) -o atemp.elf $(C_OBJ) $(S_OBJ) $(EE_LDFLAGS) $(EE_LIBS)
	ps2-packer-0.4 atemp.elf agui.elf
	rm -f atemp.elf
	mv agui.elf bin/agui.elf
	rm -f obj/*.o
	rm -f ssplash.c sMainS.c sCodeA.c sMainO.c sOptions1.c sOptions2.c sOptions3.c sStartC.c sStartS.c

	
	
	
obj/%.o: %.c
	@echo "-------------------------------------------------"
	ee-gcc $(EE_INCS) -c $(CFLAGS) $< -o $@

obj/%.o: %.s
	@echo "-------------------------------------------------"
	ee-gcc -xassembler-with-cpp -c $(CFLAGS) $< -o $@

%.c: resources/%.bmp
	@echo "-------------------------------------------------"
	bmp2c $< $(*F) > $@

clean:
	rm -f $(C_OBJ) $(S_OBJ) *.elf

ssplash.c: resources/ssplash.bmp
sMainS.c: resources/sMainS.bmp
sMainO.c: resources/sMainO.bmp
sCodeA.c: resources/sCodeA.bmp
sOptions1.c: resources/sOptions1.bmp
sOptions2.c: resources/sOptions2.bmp
sOptions3.c: resources/sOptions3.bmp
sStartC.c: resources/sStartC.bmp
sStartS.c: resources/sStartS.bmp