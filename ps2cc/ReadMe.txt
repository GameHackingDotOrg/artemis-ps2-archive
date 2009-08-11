/****************************************************************************
PS2CC ReadMe and Changelog (or a half-assed attempt at one)
*****************************************************************************/
Yeah, this is shitty and will hopefully get improved on later. I just wanted 
to note a few things.

/****************************************************************************
Changes
*****************************************************************************/
Rev 23: Automatic connection/reconnection failure issues should be resolved.
	I added a manual Reconnect option under the Run menu just in case.
	For auto reconnecting, expect a command error followed by the status
	bar updating connection info.

Rev 22: Results can now be removed from the list with the Delete key.

Rev 21: I Forgot("?") button added to Quick Search options.

Rev 20: Search labels added. Now you can name each search before executing so
	you'll know what it was.

Rev 19: Memory VIEWER is now available. Editing is planned, but I think there 
	should be an update to the PS2 side to make this a little cleaner. I'm 
	also having navigational issues with the shit. Go To works so you can 
	view everything, but keyboard navigation once you're editing is a problem.

Rev 18: I added a few of the little things from the list today, as well as 
	solving an issue that's plagued my apps since I started writing Win32 
	API. Yes, the goddamn tab key works! You'll notice a "Use Result Number" 
	combo box on the results tab now. The highlighted code(s) in the list 
	will be added to the active list with the value from that search number 
	if you hit Enter. Pressing Left/Right while navigating the results list 
	will scroll that search number box as well. The active cheats list is 
	now editable (doubleclick, enter to commit change), and toggling the 
	checkboxes using Space should update the list on the console side.

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
Page Up/Down:
	Advance the results page (the # in the combo box) up or down.

Select Result Value/Quick Activate Result: You probably noticed that clicking a 
	result copies the address and value to the input boxes for testing. What 
	you might not have noticed is that the value copied is dependant on which
	column you click in. Also, doublclicking the value will add that result
	straight to the active codes list with that value.

Multi-Select Results:
	Hold Control key. Still works on doubleclick with the values from the 
	column being clicked in. You might find you need to doubleclick a within
	a row that's NOT highlighted in order to catch the last one you select.
	Enter key also sends selected results to active list, but it uses the 
	value from the search number in the box at the bottom. This box can also
	be scrolled using the Left/Right keys while inside the results list view.

Delete Result(s):
	Result(s) can be removed by highlighting and pressing the Delete key.

Editable Active Cheats List:
	Doubleclick address/value to edit. Press Enter to commit changes. Press 
	ESC or click elsewhere to cancel.

Memory Editor:
	Viewer only at the moment. Go To works in any address, not just the top
	one. Hit enter to go to address or commit value changes. Up/Down will
	move the selection up/down.
