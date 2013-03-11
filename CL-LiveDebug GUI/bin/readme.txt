The LDv4Joker.txt is for auto loading joker addresses on boot.
That way you can get around the freezing problem with the joker scanner.
When you boot uLaunchELF, you have the option to load a pre-set joker (if auto load joker is on) based on the discs elf.
This is in case you have plans to use another booting method.

Those can be toggled in the Options menu.

Controls:
	Start: Launches uLaunchELF (mc0:/BOOT/BOOT.ELF, mc1:/BOOT/BOOT.ELF, mass:/BOOT/BOOT.ELF).
	Select: Launches OSDSYS

The rest is pretty straight forward.


 - Gtlcpimp's original readme:

www.Cheaters-Lounge.net
www.CheatersLounge.org

Loading instructions:
Insert your game disc into the PlayStation 2 console, then launch the Debugger's .ELF from any boot loader.


This program was developed to allow in-game memory viewing / editing and applying codes during game.


Enter debugger: R3 + L3 (Press at the same time)


When you create a patch, the application preserves the original data of the memory address so you can later
turn the patch "on" and "off" from the patches menu. This enables better debugging for codes created under
the application. The application currently is not finished, so some of the features may be unavailable at
this current moment. Most of the menus will display the controls on screen, for the ones that don't you can
always refer to this file for the controls.

This version has a fail-safe mode implemented into it to prevent the game from locking up when you enter the
debugger. The fail-safe mode is displayed when you enter the keys to start the debugger. It provides on-screen
instructions to enter the main menu. The reason it was implemented was due to the issue of the IOP shutting off
the controller when you first enter the debugger. If it shuts it off, then you will be unable to access anything
in the debugger (including return to game). So the fail-safe mode will give you approximately 5 seconds to enter
the debugger, if you are unable to (the IOP shut off the controller) then it will automatically return to the game
so the controller can be turned back on.

You can pre-load patches before you launch the game from a memory card or USB drive. The patches must be in
a text document named "LDv3Patches.txt", and present on the root of the drive (mc0:/ mc1:/ mass:/) for loading.
Your patches file must follow the following format:

code name or description (15 character limit)
CAAAAAAA OOOOOOOO FFFFFFFF (C: Command; A: Address; O: DataON; F: DataOFF)
{blank line}
code name or description (15 character limit)
CAAAAAAA OOOOOOOO FFFFFFFF (C: Command; A: Address; O: DataON; F: DataOFF)
{blank line}
code name or description (15 character limit)
CAAAAAAA OOOOOOOO FFFFFFFF (C: Command; A: Address; O: DataON; F: DataOFF)
{blank line}

You can have up to 512 patches to pre-load, if you have more only the first 512 will be used. Each patch (or code)
must be single line patches. An example patches file is included in RAR file you downloaded.

-= Menu controls =-

Main Menu:

Up Arrow   - Up
Down Arrow - Down
Cross      - Select


Memory Browser:

Up Arrow    - 1 Line Up
Down Arrow  - 1 Line Down
Left Arrow  - Page Up
Right Arrow - Page Down
R1          - GoTo page
Square      - Line Selector
Triangle    - Go Back


Go To:

Left Arrow  - Left
Right Arrow - Right
Up Arrow    - Increment Bit
Down Arrow  - Decrement Bit
Cross       - GoTo
Circle      - Go Back


Line Selector:

Up Arrow   - Up
Down Arrow - Down
Cross      - Edit Line
Cirlce     - Go Back


Memory Editor:

Left Arrow  - Left
Right Arrow - Right
Up Arrow    - Increment Bit
Down Arrow  - Decrement Bit
R1          - Write Data
Triangle    - Go Back


Patches Menu:

Up Arrow   - Up
Down Arrow - Down
Square     - View Patch Details
Circle     - Go Back


Patch Details:

Up Arrow   - Up
Down Arrow - Down
Cross      - Select Option


Codes Menu:

Up Arrow - Up
Down Arrow - Down
Left Arrow - Page Up
Right Arrow - Page Down
Cross - Turn Code On / Off (Font color grey-white means OFF, the other means ON)
Triangle - Exit menu


Search Menu:

Up Arrow - Up (Increment on selected option)
Down Arrow - Down (Decrement on selected option)
Left Arrow - Left on selected option (decrease bits on bit option)
Right Arrow - Right on selected option (increase bits on bit option)
Cross - Select option

Results Menu:

Up Arrow - Up
Down Arrow - Down
Left Arrow - Page Up
Right Arrow - Page Down
R2 - Go to Address in memory browser menu
Triangle - Exit menu


This release information: Version 3.0.4;

Created by: Gtlcpimp
