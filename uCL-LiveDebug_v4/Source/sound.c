/* Based off the source code from the ps2sdk's "playadpcm" */

#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <tamtypes.h>
#include <audsrv.h>
#include "sound.h"

int audsvr_start(int vol) {
	struct audsrv_fmt_t format;
	
	if (vol < 0 || vol > MAX_VOLUME) { vol = MAX_VOLUME; }
	
	int ret = audsrv_init();
	if (ret != 0)
	{
		printf("Failed to initialize audsrv\n");
		printf("audsrv returned error string: %s\n", audsrv_get_error_string());
		return -1;
	}

	format.bits = 8;
	format.freq = 11025;
	format.channels = 2;
	
	//audsrv_adpcm_init();
	audsrv_set_format(&format);
	audsrv_set_volume(vol);
	
	return 1;
}

int play_sound(void * ptr, int size) {
	//audsrv_adpcm_t sample;
	
	/*
	u8* buffer;
	buffer = malloc(size);
	memcpy(buffer, ptr, size);
	//audsrv_load_adpcm(&sample, buffer, size);
	*/
	
	//audsrv_load_adpcm(&sample, ptr, size);
	//audsrv_play_adpcm(&sample);

	audsrv_wait_audio(size);
	audsrv_play_audio((void*)ptr, size);
	//audsrv_stop_audio();
	
	//free(buffer);
	
	return 1;
}

int _play_sound(void * ptr, int size) {
	
	
	
	return 1;
}
