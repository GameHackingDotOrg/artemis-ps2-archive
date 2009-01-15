/**
 * Copyright (C) 2008 misfire <misfire@xploderfreax.de>
 *
 *  - Win32 port: include missing elf.h header file
 *  - Added irxtool version info.
 *  - Fixed the version number output in showImport() and showExport().  A value
 *    of 0x0101 is now translated to the string "1.1", and not to "1.01".
 *    Similarly, 0x020B corresponds to "2.11".
 *
 * Copyright 2005 Mega Man
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#ifdef _WIN32
	#include "elf.h"
#else
	#include <elf.h>
#endif

#define IRXTOOL_VERSION "1.1"

#define IRX_MAGIC_IMPORT 0x41e00000
#define IRX_MAGIC_EXPORT 0x41c00000

#define IRX_MAGIC_IMPORT_FUNCTION 0x03E00008

typedef struct modulefunction {
	const char *name;
	int nr;
	const char *functionname;
} modulefunction_t;

modulefunction_t modulesdb[] = {
#include "import.db"
	{ NULL, 0, NULL }
};

/** Find virtual address of file offset. */
int findAddress(int offset);

int mapformat = 0;

const char *findFunction(const char *modulename, int nr)
{
	int i;

	i = 0;
	while(modulesdb[i].name != NULL) {
		if (strcmp(modulesdb[i].name, modulename) == 0) {
			if (modulesdb[i].nr == nr)
				return modulesdb[i].functionname;
		}
		i++;
	}
	return NULL;
}

void showImportFunctions(FILE *fin, const char *modulename)
{
	do {
		int magic;
		int number;
		const char *name;
		int addr;

		addr = ftell(fin);

		// read magic, jump command:
		if (fread(&magic, sizeof(int), 1, fin) != 1)
			return;
		if (magic != IRX_MAGIC_IMPORT_FUNCTION)
			return;

		if (fread(&number, sizeof(int), 1, fin) != 1)
			return;

		name = findFunction(modulename, number & 0x00FFFFFF);
		addr = findAddress(addr);
		if (mapformat) {
			if (name != NULL)
				printf("0x%08x %s\n", addr, name);
			else
				printf("0x%08x %s_%d\n", addr, modulename, number & 0x00FFFFFF);
		} else {
			if (name != NULL)
				printf("0x%08x %d %s()\n", addr, number & 0x00FFFFFF, name);
			else
				printf("0x%08x %d\n", addr, number & 0x00FFFFFF);
		}
	} while(1);
}

void showImport(FILE *fin)
{
	int version;
	char modulename[9];

	// unused:
	if (fread(&version, sizeof(int), 1, fin) != 1)
		return;

	// Read version number
	if (fread(&version, sizeof(int), 1, fin) != 1)
		return;

	// Read version number
	if (fread(modulename, 8, 1, fin) != 1)
		return;

	modulename[8] = 0;
	if (!mapformat)
		printf("\n%s %d.%d\n", modulename,
			(version >> 8) & 0xff,
			version & 0xff);
	showImportFunctions(fin, modulename);
	
}

void showExportFunctions(FILE *fin, const char *modulename)
{
	char temp[256];
	int number = 0;

	do {
		int addr;
		int offset;
		const char *name;

		offset = ftell(fin);

		// read magic, jump command:
		if (fread(&addr, sizeof(int), 1, fin) != 1)
			return;
		if (addr == 0)
			return;

		name = findFunction(modulename, number);
		if (name == NULL
			&& number == 0) {
			name = temp;
			snprintf((char*)name, 256, "%s_start", modulename);
		}
		offset = findAddress(offset);
		if (mapformat) {
			if (name != NULL)
				printf("0x%08x %s\n", addr, name);
			else
				printf("0x%08x %s_%d\n", addr, modulename, number);
		} else {
			if (name != NULL)
				printf("0x%08x %d %s() at 0x%08x\n", offset, number, name, addr);
			else
				printf("0x%08x %d at 0x%08x\n", offset, number, addr);
		}
		number++;
	} while(1);
}

void showExport(FILE *fin)
{
	int version;
	char modulename[9];

	// unused:
	if (fread(&version, sizeof(int), 1, fin) != 1)
		return;

	// Read version number
	if (fread(&version, sizeof(int), 1, fin) != 1)
		return;

	// Read version number
	if (fread(modulename, 8, 1, fin) != 1)
		return;

	modulename[8] = 0;
	if (!mapformat)
		printf("\nmodule %s %d.%d exported functions:\n", modulename,
			(version >> 8) & 0xff,
			version & 0xff);
	
	showExportFunctions(fin, modulename);
}

Elf32_Ehdr file_header;
Elf32_Phdr *program_header;

int findAddress(int addr)
{
	int i;

	for (i = 0; i < file_header.e_phnum; i++) {
		if (program_header[i].p_type == PT_LOAD)
		{
			if ((addr >= program_header[i].p_offset)
				|| (addr < (program_header[i].p_offset + program_header[i].p_filesz))) {
				return (addr - program_header[i].p_offset) + program_header[i].p_vaddr;

			}
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	FILE *fin;
	const char *filename = NULL;

	if (argc == 3) {
		if (strcmp(argv[1], "-m") == 0) {
			mapformat = -1;
			filename = argv[2];
		}
	}
	if (argc == 2)
		filename = argv[1];

	if (filename == NULL) {
		fprintf(stderr, "Usage: %s [-m] [file name of irx module]\n\n", argv[0]);
		fprintf(stderr, "Print imported and exported modules and functions of irx files.\n");
		fprintf(stderr, "The value in the first column is the virtual address where the\n");
		fprintf(stderr, "import or export is defined.\n");
		exit(1);
	}

	fin = fopen(filename, "rb");
	if (fin != NULL)
	{
		int magic;
		int i;

		if (fread(&file_header, sizeof(file_header), 1, fin) != 1) {
			fprintf(stderr, "File is too small.\n");
			fclose(fin);
			exit(1);
		}
		if (file_header.e_ident[0] != ELFMAG0
			|| file_header.e_ident[1] != ELFMAG1
			|| file_header.e_ident[2] != ELFMAG2
			|| file_header.e_ident[3] != ELFMAG3) {
			fprintf(stderr, "File is not a elf file.\n");
			fclose(fin);
			exit(1);
		}
		if (file_header.e_machine != EM_MIPS) {
			fprintf(stderr, "Wrong architecture %d!\n", file_header.e_machine);
			fclose(fin);
			exit(1);
		}
		program_header = (Elf32_Phdr *) malloc(sizeof(Elf32_Phdr) * file_header.e_phnum);
		for (i = 0; i < file_header.e_phnum; i++) {
			if (fread(program_header, sizeof(Elf32_Phdr), 1, fin) != 1) {
				fprintf(stderr, "File is too small.\n");
				fclose(fin);
				exit(1);
			}
		}
		while(fread(&magic, sizeof(int), 1, fin) == 1) {
			if (magic == IRX_MAGIC_IMPORT)
				showImport(fin);
			if (magic == IRX_MAGIC_EXPORT)
				showExport(fin);
		}
		free(program_header);
		fclose(fin);
	}
	else
	{
		fprintf(stderr, "Can't open file \"%s\".\n", filename);
		exit(1);
	}
	return 0;
}

