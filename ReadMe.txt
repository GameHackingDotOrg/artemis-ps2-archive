This is just an attempt at testing out some fonts for use in Project Artemis (the PS2 hacking/code creating system in development at GSHI.org, in case you've stumbled upon this somewhere other than GSHI).

For the time being (and perhaps permanently), I'm going to be using DreamTime's g2 graphics library, rather than gsKit or gslib. This is primarily because DreamTime's already done 90% of the work, by implementing a decent method of drawing fonts via g2 (using condensed bitmap fonts converted to C arrays), and I'd rather not reinvent the wheel.

Of course, thanks goes out to DreamTime, for g2, PS2Tut (which explains quite a bit of g2, and bitmap images and fonts), BMP2C, and fixed_tc.c :)

I know this demo is badly written, and I should fix such things as the simple C increment-based delays, and implement the pads (controllers) to transition between samples. I'll do that next, along with some basic layouts for the various screens of the Artemis PS2-side startup GUI.

Anyway, let me know which fonts you like, and keep in mind that it's quite simple to change sizes and colors, make fonts bold, italic, or underlined, add new fonts, and so forth.

Also, the included ELF was packed with PS2-Packer 0.41, hence the 66 KB file size, rather than the original 1.5 MB so, heh.