//---------------------------------------------------------------------------
// Current File: Artemis_Fonts.c
// Current Author: Lazy Bastard, LBGSHI@gmail.com
// Original File:	demo2c.c
// Original Author:	Tony Saveski, t_saveski@yahoo.com
//
// This source is based on a demo by Tony Saveski (part of his well-known PS2TUT), and utilizes his g2 graphics
// library, as well as his BMP2C application. Without his work, I would have been forced to use gsKit or gslib,
// or to just figure out how to manually force the PS2 to draw letters. Be sure to send him an email to thank
// him for all of his work in the PS2 development scene :)
//---------------------------------------------------------------------------
#include "g2.h"
#include "gs.h"

// good fonts
extern uint32 BankGothicLtBT[];
extern uint32 BankGothicMdBT[];
extern uint32 BernardMTCondensed[];
extern uint32 Consolas[];
extern uint32 Gloucester[];
extern uint32 NiagaraSolid[];
extern uint32 OCRAExtended[];
extern uint32 Proxy7[];
extern uint32 Swis721BDCNOUL[];
extern uint32 TempusSansITC[];
extern uint32 TimesNewRoman[];
extern uint32 VinerHandITC[];
extern uint32 VinetaBT[];
extern uint32 WideLatin[];

extern uint16 fixed_tc[];


static uint16 maxx, maxy;

//---------------------------------------------------------------------------
void flip_buffers(void)
{
	g2_wait_vsync();
	g2_set_visible_frame(1 - g2_get_visible_frame());
	g2_set_active_frame(1 - g2_get_active_frame());
}


//---------------------------------------------------------------------------
int main(int argc, char **argv)
{
uint32 i;

	argc=argc;
	argv=argv;

	if(gs_is_ntsc())
		g2_init(NTSC_512_224_32);
	else
		g2_init(PAL_512_256_32);

	maxx = g2_get_max_x();
	maxy = g2_get_max_y();

	// clear the screens and make sure visible and active
	// buffers are different.
	g2_set_active_frame(0);
	g2_set_fill_color(0, 0, 0);
	g2_fill_rect(0, 0, maxx, maxy);
	g2_set_visible_frame(0);

	g2_set_active_frame(1);
	g2_set_fill_color(0, 0, 0);
	g2_fill_rect(0, 0, maxx, maxy);

	while(1)
	{
		
		g2_set_visible_frame(1);
		g2_set_active_frame(0);
		g2_set_visible_frame(0);

		
		// BankGothicLtBT Fixed Intro
		g2_set_font(BankGothicLtBT, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(1);

		g2_out_text(8, 65,  "This is a quick demo to showcase various");
		g2_out_text(8, 80,  "fonts we could use for Artemis. Let us know");
		g2_out_text(8, 95,  "what you think, at the GSHI.org forums!");
		g2_out_text(8, 110,  "Keep in mind, we can change the colors to");
		g2_out_text(8, 125,  "whatever we like; and use different fonts;");
		g2_out_text(8, 140,  "these are just examples.");
		for(i=0; i<0x8FFFFFF; i++) {} // delay
		
//--------------------------------------------------------------------------

		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// BankGothicLtBT Fixed Artemis demo
		g2_set_font(BankGothicLtBT, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(145, 125,  "(Bank Gothic Lt Bt)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//--------------------------------------------------------------------
		
		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// BankGothicMdBT Fixed Artemis demo
		g2_set_font(BankGothicMdBT, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(145, 125,  "(Bank Gothic Md BT)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//-----------------------------------------------------------------------
		
		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// BernardMTCondensed Fixed Artemis demo
		g2_set_font(BernardMTCondensed, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(130, 125,  "(Bernard MT Condensed)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//-----------------------------------------------------------------------
		
		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// Consolas Fixed Artemis demo
		g2_set_font(Consolas, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(190, 125,  "(Consolas)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//-----------------------------------------------------------------------
		
		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// Gloucester Fixed Artemis demo
		g2_set_font(Gloucester, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(181, 125,  "(Gloucester)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//-----------------------------------------------------------------------
		
		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// NiagaraSolid Fixed Artemis demo
		g2_set_font(NiagaraSolid, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(169, 125,  "(Niagara Solid)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//-----------------------------------------------------------------------		

		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// OCRAExtended Fixed Artemis demo
		g2_set_font(OCRAExtended, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(158, 125,  "(OCR A Extended)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//--------------------------------------------------------------------

		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// Proxy7 Fixed Artemis demo
		g2_set_font(Proxy7, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(190, 125,  "(Proxy 7)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//--------------------------------------------------------------------

		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// Swis721BDCNOUL Fixed Artemis demo
		g2_set_font(Swis721BDCNOUL, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(145, 125,  "(Swis 721 BDCNOUL)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//--------------------------------------------------------------------

		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// TempusSansITC Fixed Artemis demo
		g2_set_font(TempusSansITC, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(145, 125,  "(Tempus Sans ITC)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//--------------------------------------------------------------------

		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// TimesNewRoman Fixed Artemis demo
		g2_set_font(TimesNewRoman, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(145, 125,  "(Times New Roman)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//--------------------------------------------------------------------

		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// VinerHandITC Fixed Artemis demo
		g2_set_font(VinerHandITC, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(150, 125,  "(Viner Hand ITC)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//--------------------------------------------------------------------
		
		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// VinetaBT Fixed Artemis demo
		g2_set_font(VinetaBT, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(165, 125,  "(Vineta BT)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//-----------------------------------------------------------------------
		
		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		// WideLatin Fixed Artemis demo
		g2_set_font(WideLatin, 256, 128, fixed_tc);
		g2_set_font_spacing(-5);
		g2_set_font_mag(2);
		g2_out_text(165, 40,  "Artemis");
		g2_set_font_mag(1);
		g2_out_text(100, 75,  "Playstation 2 Hacking System");
		g2_out_text(165, 125,  "(Wide Latin)");
		for(i=0; i<0x5FFFFFF; i++) {} // delay
		
//-----------------------------------------------------------------------

		
		//Clear screen
		g2_set_active_frame(0);
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
		g2_set_visible_frame(0);

		g2_set_active_frame(1);

	}

	// ok...it will never get here...
	g2_end();
	return(0);
}
