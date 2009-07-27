/****************************************************************************
PS2CC ReadMe and Changelog (or a half-assed attempt at one)
*****************************************************************************/
Yeah, this is shitty and will hopefully get improved on later. I just wanted 
to note a few things.

/****************************************************************************
Changes
*****************************************************************************/
Rev 17: lib_ntpb.c completely redone so the interface no longer "freezes" while 
	dumping. You still shouldn't be trying to do anything else with it though.
	Halt and Resume options have also been added, as well as a "PS2 Waits"
	checkbox on the search tab. This makes the PS2 stay halted until the 
	search is actually processed. You can also halt the system beforehand
	and uncheck the box so the PS2 will stay halted even after searching is
	done. Could be useful with some timers or things the end quickly and
	need multiple dumps while letting very little time progress in the game.
	F4 and F5 are also shortcut keys for halt and resume, as you'll notice in
	the menu.

Rev 16: Results testing should allow 8, 16, and 32-bit writes now. Multi-select
	is also enabled on the results list.

Rev 15: Added Page Up/Down functionality on the results display. Note that this 
	shifts the actual page, as opposed to scroling the box, so set your page 
	sizes as desired.

Rev 14: 'I Fogot' search type was reporting 0 results. Luckily, it just seemed to 
	be a lack of the count itself being copied over.

/****************************************************************************
Keyboard Shortcuts and little known features (ones that I remember)
*****************************************************************************/
Page Up/Down: Advance the results page (the # in the combo box) up or down.

Select Result Value/Quick Activate Result: You probably noticed that clicking a 
	result copies the address and value to the input boxes for testing. What 
	you might not have noticed is that the value copied is dependant on which
	column you click in. Also, doublclicking the value will add that result
	straight to the active codes list with that value.
Multi-Select Results:
	Hold Control key. Still works on doubleclick with the values from the 
	column being clicked in. You might find you need to doubleclick a within
	a row that's NOT highlighted in order to catch the last one you select.
