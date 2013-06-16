#include <string.h>
#include <tamtypes.h>
#include <stdio.h>
#include <libconfig.h>
#include <libpad.h>
#include "config.h"

extern config_t config;
#define J_SIZE 10

typedef struct {
	int val;
	char *name;
} structPad;

structPad PadArray[16] = {
	{ PAD_SELECT, "SEL" },
	{ PAD_L3, "L3" },
	{ PAD_R3, "R3" },
	{ PAD_START, "STRT" },
	{ PAD_UP, "UP" },
	{ PAD_RIGHT, "RGHT" },
	{ PAD_DOWN, "DWN" },
	{ PAD_LEFT, "LFT" },
	{ PAD_L2, "L2" },
	{ PAD_R2, "R2" },
	{ PAD_L1, "L1" },
	{ PAD_R1, "R1" },
	{ PAD_TRIANGLE, "/\\" },
	{ PAD_CIRCLE, "O" },
	{ PAD_CROSS, "X" },
	{ PAD_SQUARE, "[]" }
};

/* Parses a joker value into the corresponding buttons with a '+' in between them */
int ParseJokerVal(int val, char *ret) {
	char *retStr = malloc(sizeof(ret));
	memset(retStr, 0, sizeof(retStr));
	int x = 0;
	
	for (x = 0; x < 16; x++) {
		if (val & PadArray[x].val) {
			sprintf(retStr + strlen(retStr), "%s+", PadArray[x].name);
		}
		
	}
	
	if (retStr != NULL) {
		strcpy(ret, retStr);
		ret[strlen(ret)-1] = 0; //Get rid of the unnecessary '+'
		return 1;
	}
	else
		return 0;
}

/* Just here for future reference (if needed)
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
			free(result);  // How can this be freed and then returned?
			return result; // I must have done this when I was tired. But how does this work??
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
*/

//Load joker loads the joker from the joker_array
const char *LoadJoker(char *elfname, char *ver) {
	static const char *s = NULL;
	int i = 0;
	s = config_get_string_elem(&config, SET_JOKER_ARRAY, i);
	while (s != NULL) {
		if (strcmp(elfname, s) == 0) { 
			s = config_get_string_elem(&config, SET_JOKER_ARRAY, i + 2);
			if (s == NULL || strcmp(s, "") == 0 || strcmp(s, ver) == 0)
				return config_get_string_elem(&config, SET_JOKER_ARRAY, i + 1); 
		}
		i += 3;
		s = config_get_string_elem(&config, SET_JOKER_ARRAY, i);
	}
	
	return NULL;
}

//Joker array (pad address), must end with { "", "" }
//GH stands for Greatest Hits
char *jaddr[(int)J_SIZE][3] = {
	{ "SCUS_974.65", "0x001EE682", "1.00" }, //Ratchet: Deadlocked (GH)
	{ "SLUS_211.94", "0x0084305C", "" }, //Phantasy Star Universe
	{ "SCUS_971.24", "0x0012E842", "" }, //Jak 1
	{ "SLUS_205.71", "0x0058D19C", "" }, //Ty the Tasmanian Tiger
	{ "SLUS_214.93", "0x00300F82", "" }, //Need For Speed Carbon
	{ "SLUS_212.67", "0x00AAE882", "" }, //Need For Speed: Most Wanted NTSC U/C (GH)
	{ "SLUS_209.52", "0x005237C2", "" }, //Tak 2: The Staff Of Dreams
	{ "SLUS_209.63", "0x0056BADC", "" }, //Final Fantasy XII
	
	{ "", "", "" } //Do not delete!
	//{ "", "", "" },
};

char *LoadJokerArray(char *elf, char *ver) {
	int x = 0;
	while (x < (int)J_SIZE && (strcmp(jaddr[x][0], "") != 0)) {
		if (strcmp(jaddr[x][0], elf) == 0 &&
			(strcmp(jaddr[x][2], "") == 0 || strcmp(jaddr[x][2], ver) == 0))
				return jaddr[x][1];
		x++;
	}
	return NULL;
}
