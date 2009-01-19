#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
	#define ROM_PADMAN
	#endif

	#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
	#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
	#endif

	#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
	#error ROM_PADMAN or NEW_PADMAN must be defined!
	#endif
	//pad buffer
	static char padBuf[256] __attribute__((aligned(64)));
	//rumblers
	static char actAlign[6];
	static int actuators;
	//button status
	struct padButtonStatus buttons;
		u32 paddata;
		u32 old_pad;
		u32 new_pad;
	int port, slot;