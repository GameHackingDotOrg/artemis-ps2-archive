/*
 * Configuration manager
 *
 * Copyright (C) 2009-2010 Mathias Lafeldt <misfire@debugon.org>
 *
 * This file is part of PS2rd, the PS2 remote debugger.
 *
 * PS2rd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PS2rd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PS2rd.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
 */
 
 /*
  * If you didn't already guess, this is from PS2rd
  * So many thanks to misfire for porting libconfig
  */

#include <tamtypes.h>
#include <stdio.h>
#include <libconfig.h>
#include "config.h"
#include "debug.h"

//Directly from PS2rd's configman.c
/**
 * config_build - Build configuration with all required settings.
 * @config: ptr to config
 */
void config_build(config_t *config)
{

	/*
		NOTE: If you add anything to this, please put it before the sgfix and joker arrays within the conf file.
		Then in the main.c go to the function StoreSett and add in your new setting
	*/
	
	config_setting_t *root, *group, *set;

	config_init(config);
	root = config_root_setting(config);
	
	/*
	 * main section
	 */
	group = config_setting_add(root, "main", CONFIG_TYPE_GROUP);

	set = config_setting_add(group, "preset_joker", CONFIG_TYPE_BOOL);
	config_setting_set_bool(set, (int)SET_PRESET_JOKER);
	
	set = config_setting_add(group, "preset_ule", CONFIG_TYPE_BOOL);
	config_setting_set_bool(set, (int)SET_PRESET_ULE);
	
	#ifdef EH
		set = config_setting_add(group, "install_ehandler", CONFIG_TYPE_BOOL);
		config_setting_set_bool(set, (int)SET_INSTALL_EHANDLER);
	#endif
	
	set = config_setting_add(group, "ule_path", CONFIG_TYPE_STRING);
	config_setting_set_string(set, (char*)SET_ULE_PATH);

	
	/*
	 * Debugger section
	 */
	group = config_setting_add(root, "debugger", CONFIG_TYPE_GROUP);
	
	set = config_setting_add(group, "pkt_alloc", CONFIG_FORMAT_HEX);
	config_setting_set_int(set, (int)SET_PACKET_ALLOC);
	
	#ifdef EH
		set = config_setting_add(group, "eh_alloc", CONFIG_FORMAT_HEX);
		config_setting_set_int(set, (int)SET_EXC_HAND_ALLOC);
	#endif
	
	set = config_setting_add(group, "regd_alloc", CONFIG_FORMAT_HEX);
	config_setting_set_int(set, (int)SET_REG_DUMP_ALLOC);
	
	set = config_setting_add(group, "jok_combo", CONFIG_FORMAT_HEX);
	config_setting_set_int(set, (int)SET_JOK_COMBO);
	
	/*
	 * JokerArray section
	 */
	group = config_setting_add(root, "joker", CONFIG_TYPE_GROUP);
	set = config_setting_add(group, "joker_array", CONFIG_TYPE_ARRAY);
	
}

/**
 * config_print - Print out all config settings.
 * @config: ptr to config
 */
void config_print(const config_t *config)
{
	const char *s = NULL;
	int i;

	printf("config settings:\n");

#define PRINT_BOOL(key) \
	printf("%s = %i\n", key, config_get_bool(config, key))
#define PRINT_INT(key) \
	printf("%s = %i\n", key, config_get_int(config, key))
#define PRINT_HEX(key) \
	printf("%s = %08x\n", key, config_get_int(config, key))
#define PRINT_STRING(key) \
	printf("%s = %s\n", key, config_get_string(config, key))
#define PRINT_STRING_ARRAY(key) \
	i = 0; \
	do { \
		s = config_get_string_elem(config, key, i); \
		printf("%s[%i] = %s\n", key, i, s); \
		i++; \
	} while (s != NULL)
	
	/* loader */
	PRINT_BOOL(SET_PRESET_JOKER);
	PRINT_BOOL(SET_PRESET_ULE);
	#ifdef EH
		PRINT_BOOL(SET_INSTALL_EHANDLER);
	#endif
	PRINT_STRING(SET_ULE_PATH);
	
	/* Debugger */
	PRINT_HEX(SET_PACKET_ALLOC);
	PRINT_HEX(SET_EXC_HAND_ALLOC);
	PRINT_HEX(SET_REG_DUMP_ALLOC);
	PRINT_HEX(SET_JOK_COMBO);
	
	/* Joker array */
	PRINT_STRING_ARRAY(SET_JOKER_ARRAY);
	
}
