#include <string.h>
#include <tamtypes.h>
#include <stdio.h>

u32 TextToCode(char *text) {
	u32 code = 0;
	u8 b1, b2, b3, b4, b5, b6, b7, b8;
	
	if (text == NULL) { return 0; }
	
	b1 = text[0];
	b2 = text[1];
	b3 = text[2];
	b4 = text[3];
	b5 = text[4];
	b6 = text[5];
	b7 = text[6];
	b8 = text[7];

	b1 -= 0x30;
	b2 -= 0x30;
	b3 -= 0x30;
	b4 -= 0x30;
	b5 -= 0x30;
	b6 -= 0x30;
	b7 -= 0x30;
	b8 -= 0x30;

	// Lower Case                          Upper Case
	if (b1 > 0x30) { b1 -= 0x27; } else if (b1 > 0x10) { b1 -= 0x07; }
	if (b2 > 0x30) { b2 -= 0x27; } else if (b2 > 0x10) { b2 -= 0x07; }
	if (b3 > 0x30) { b3 -= 0x27; } else if (b3 > 0x10) { b3 -= 0x07; }
	if (b4 > 0x30) { b4 -= 0x27; } else if (b4 > 0x10) { b4 -= 0x07; }
	if (b5 > 0x30) { b5 -= 0x27; } else if (b5 > 0x10) { b5 -= 0x07; }
	if (b6 > 0x30) { b6 -= 0x27; } else if (b6 > 0x10) { b6 -= 0x07; }
	if (b7 > 0x30) { b7 -= 0x27; } else if (b7 > 0x10) { b7 -= 0x07; }
	if (b8 > 0x30) { b8 -= 0x27; } else if (b8 > 0x10) { b8 -= 0x07; }

	code = 0;
	code += (b8 * 0x1);
	code += (b7 * 0x10);
	code += (b6 * 0x100);
	code += (b5 * 0x1000);
	code += (b4 * 0x10000);
	code += (b3 * 0x100000);
	code += (b2 * 0x1000000);
	code += (b1 * 0x10000000);
	
	return code;
}

char *LoadJoker(char *elfname, char *file) {
	int fd = 0, size = 0, x = 0, y = 0, z = 0;
	char *result, *temp;
	
	fd = fioOpen(file, O_RDONLY);
	if (fd < 0) { fioClose(fd); return NULL; }
	
	size = fioLseek(fd, 0, SEEK_END);
	fioLseek(fd, 0, SEEK_SET);
	if (size < 0) { fioClose(fd); return NULL; }
	
	result = malloc(size + 1);
	temp = malloc(2);
	
	fioRead(fd, result, size);
	fioClose(fd);
	
	while (z < (size - 1)) {
		while (result[x] != ' ') { x++; }
		temp[0] = result[x];
		result[x] = '\0';

		if (strcmp(result, elfname) == 0) {
			result[x] = temp[0];
			y = x;
			while (result[y] != '\n') { y++; }
			result[y] = '\0';
			result += x + 1;
			free(temp);
			free(result);
			return result;
		}
		else {
			result[x] = temp[0];
			while (result[x] != '\n') { x++; }
			result += x + 1;
			z += x;
			x = 0;
		}
	}
	free(temp);
	free(result);
	return NULL;
}

//Joker array (pad address), must end with { "", "" }
char *padaddr[7][2] = {
	{ "SCUS_974.65", "001EE682" }, //Ratchet: Deadlocked
	{ "SLUS_211.94", "0084305C" }, //Phantasy Star Universe
	{ "SCUS_971.24", "0012e842" }, //Jak 1
	{ "SLUS_205.71", "0058D19C" }, //Ty the Tasmanian Tiger
	{ "SLUS_214.93", "00300F82" }, //Need For Speed Carbon
	{ "SLUS_212.67", "00AAE882" }, //Need For Speed: Most Wanted NTSC U/C (Greatest Hits)
	{ "", "" }, //Do not delete!
	//{ "", "" },
};

char *LoadJokerArray(char *elf) {
	int x = 0;
	
	while (x < (sizeof(padaddr) / sizeof(padaddr[0])) && (strcmp(padaddr[x][0], "") != 0)) {
		if (strcmp(padaddr[x][0], elf) == 0) { return padaddr[x][1]; }
		x++;
	}
	return NULL;
}
