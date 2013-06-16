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

#ifndef _CONFIGMAN_H_
#define _CONFIGMAN_H_

//Uncomment if you want to include the Exception Handler in the config file
//#define EH

#include <tamtypes.h>
#include <libconfig.h>

/* Keys to access different settings in configuration */
#define SET_PRESET_JOKER		"main.preset_joker"
#define SET_PRESET_ULE			"main.preset_ule"
#define SET_INSTALL_EHANDLER	"main.install_ehandler"
#define SET_ULE_PATH			"main.ule_path"

#define SET_PACKET_ALLOC		"debugger.pkt_alloc"
#define SET_EXC_HAND_ALLOC		"debugger.eh_alloc"
#define SET_REG_DUMP_ALLOC		"debugger.regd_alloc"
#define SET_JOK_COMBO			"debugger.jok_combo"

#define SET_JOKER_ARRAY			"joker.joker_array"

void config_build(config_t *config);
void config_print(const config_t *config);

/*
 * libconfig wrapper functions for lazy people.
 */

static inline int config_get_int(const config_t *config, const char *path)
{
	return config_setting_get_int(config_lookup(config, path));
}

static inline long long config_get_int64(const config_t *config, const char *path)
{
	return config_setting_get_int64(config_lookup(config, path));
}

static inline double config_get_float(const config_t *config, const char *path)
{
	return config_setting_get_float(config_lookup(config, path));
}

static inline int config_get_bool(const config_t *config, const char *path)
{
	return config_setting_get_bool(config_lookup(config, path));
}

static inline const char *config_get_string(const config_t *config, const char *path)
{
	return config_setting_get_string(config_lookup(config, path));
}

static inline const char *config_get_string_elem(const config_t *config, const char *path, int index)
{
	return config_setting_get_string_elem(config_lookup(config, path), index);
}

#endif /* _CONFIGMAN_H_ */
