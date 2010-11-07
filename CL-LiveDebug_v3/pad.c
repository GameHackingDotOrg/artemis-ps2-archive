

#define ROM_PADMAN
#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
#endif
#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#error ROM_PADMAN or NEW_PADMAN must be defined!
#endif

static char padBuf[256] __attribute__((aligned(64)));
static char actAlign[6];
static int actuators;

int waitPadReady(int port, int slot)
{
	int state;
	int lastState;
	char stateString[16];

	state = padGetState(port, slot);
	lastState = -1;
	while( (state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1) )
	{
		if (state != lastState)
		{
			padStateInt2String(state, stateString);
		}

		lastState = state;
		state = padGetState(port, slot);
	}

	return 0;
}

int initializePad(int port, int slot)
{

	int ret;
	int modes;
	int i;

	waitPadReady(port, slot);

	modes = padInfoMode(port, slot, PAD_MODETABLE, -1);

	/*
	if (modes > 0)
	{
		for (i = 0; i < modes; i++)
		{
			ret = padInfoMode(port, slot, PAD_MODETABLE, i);
		}
	}
	*/

	if (modes == 0)
		return 1;

	i = 0;
	do
	{
		if ( padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK )
			break;
		i++;
	} while (i < modes);

	if (i >= modes)
		return 1;

	ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);

	if (ret == 0)
		return 1;

	padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

	waitPadReady(port, slot);
	ret = padInfoPressMode(port, slot);

	waitPadReady(port, slot);		
	ret = padEnterPressMode(port, slot);

	waitPadReady(port, slot);
	actuators = padInfoAct(port, slot, -1, 0);

	if (actuators != 0)
	{
		actAlign[0] = 0;	// Enable small engine
		actAlign[1] = 1;	// Enable big engine
		actAlign[2] = 0xff;
		actAlign[3] = 0xff;
		actAlign[4] = 0xff;
		actAlign[5] = 0xff;

		waitPadReady(port, slot);
		ret = padSetActAlign(port, slot, actAlign);
	}

	waitPadReady(port, slot);

	return 1;
}
