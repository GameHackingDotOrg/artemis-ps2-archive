#include <tamtypes.h>
#include <kernel.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <loadfile.h>
#include <png.h>
#include <string.h>
#include <libpad.h>

extern int DEBUG;

extern void load_elf(char *elf_path);

struct about_content {
	char *name;
	char *desc;
};

/* fn prototypes */
void Setup_GS(int gs_vmode);
void gfx_set_defaults(void);
void vram_free(void);
void Clear_Screen(void);
int  Draw_INTRO_part1(void);
int  Draw_INTRO_part2(void);
int  Draw_MainMenu(int selected_button, int highlight_pulse);
int Draw_OptionsMenu(void);
int  Init_AboutMenu(void);
int  Draw_AboutMenu(char *version, struct about_content *about_text);
void Render_GUI(void);
int LoadPatches(int m);
int DrawMenuBar(int X);
int draw_keyBoard(char *ret, int mode);
int draw_kb_chars(void);
int draw_kb_hex(void);
int draw_kb_frame(void);
int draw_kb_frame_hex(void);
int draw_YorN(char *Q, char *Yes, char *No);
int draw_jokerCombo(char *title1, char *title2);
int Draw_PatchesMenu(void);

/* screen defaults for NTSC, just in case */
int TV_mode       = 2;
int	SCREEN_WIDTH  = 640;
int	SCREEN_HEIGHT = 448;
int SCREEN_X	  = 632;
int SCREEN_Y	  = 50;
float Y_RATIO	  = 0.875f;

/* define colors */
#define Black  		GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00)

/* For GUI */
int background_alpha;
int logo_alpha;
int icon_start_y;
int icon_stop_y;
int icon_about_y;
int icon_options_y;
int icon_patches_y;
int icon_start_alpha;
int icon_stop_alpha;
int icon_about_alpha;
int icon_options_alpha;
int icon_patches_alpha;
int icon_mc_alpha = 0;
int icon_mass_alpha = 0;
int desc_start_alpha;
int desc_stop_alpha;
int desc_about_alpha;
int desc_options_alpha;
int desc_patches_alpha;
int desc_mc0_alpha = 0;
int desc_mc1_alpha = 0;
int desc_mass_alpha = 0;
int highlight_alpha;
int amount;
int pause_pulse;

int desc_stop_y = 360;
int desc_start_x = 42;
int desc_stop_x = 140;
int desc_patches_x = 245;
int desc_options_x = 342;
int desc_about_x = 440;

int icon_start_x = 64;
int icon_stop_x = 160;
int icon_patches_x = 265;
int icon_options_x = 360;
int icon_about_x = 460;

//About menu
int about_menu_header_alpha;
int about_menu_thx_alpha;
int about_menu_content_alpha;

//Options menu
int optselbutt = 1;
int joker_alpha = 0;
int ule_alpha = 0;
int exc_alpha = 0;
int uboot_alpha = 0;
int butt_alpha = 0;
int add_pj_alpha = 0; //Alpha of text "-> Add New Preset Joker <-"
int dc_alpha = 0;
int jokUpdate = 0;
char jokComStr[100];
char valStr[100];
#define opt_max 6 //Max amount of options

//Patches menu
char *patchret = NULL;
int patch_x = 0;
int patches_image_alpha = 0;
int options_text_alpha = 0;
int selbutt = 1;

//For menu bar animation
int rate = 12;
int menu_bar_x = 0;
int menu_bar_y = 60;
int menu_bar_a = 128;

/* For the keyboard */
//Lowercase
char *keysL[53] = { "`", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",				//Row 1
					"TAB", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]",				//Row 2
					"CAPS", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "ENTER",		//Row 3
					"SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", ":/", "CANCEL",		//Row 4
													"SPACE"											//Row 5
};
						
//Uppercase
char *keysU[53] = { "\\", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+",				//Row 1
					"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}",				//Row 2
					"CAPS", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\'", "ENTER",		//Row 3
					"SHIFT", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", ":/", "CANCEL",		//Row 4
													"SPACE"											//Row 5
};

//Hexadecimal
char *keysH[18] = { "0", "1", "2", "3", "4", "5", "6", "7",											//Row 1
					"8", "9", "A", "B", "C", "D", "E", "F",											//Row 2
							"ENTER", 		"CANCEL"												//Row 3
};

int kb_highl[53] = { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
					 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
					 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
					 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
					 128
};

int sent = 0;

const int row = 13;             //Number of columns
const int CANC = (13 * 3) + 12; //Cancel
const int H_CANC = 18;			//Hexadecimal Cancel
const int ENT = (13 * 2) + 12;  //Enter
const int H_ENT = 17;			//Hexadecimal Enter
const int TAB = (13 * 1);       //Tab
const int CAPS = (13 * 2);      //Caps lock
const int SHIFT = (13 * 3);     //Shift
const int SPEC1 = (13 * 3) + 11;

//Shift and caps lock
int kb_shift = 0;
int kb_caps = 0;

/* You may change the frame_x and frame_y values and everything else will move accordingly */
int kb_frame_y = 120;
int kb_frame_x = 4;
int kb_frameH_y = 120;
int kb_frameH_x = 4;

int key_yoff = 7;
int key_xoff = 10;
int key_yoffH = 6;
int key_xoffH = 10;

/* These will be defined within the main function (main.c) */
int kb_yoff = 0, kb_yrate = 0, kb_yspace = 0;
int kb_xoff = 0, kb_xrate = 0, kb_xspace = 0;
int kb_yoffH = 0, kb_yrateH = 0, kb_yenterH = 0;
int kb_xoffH = 0, kb_xrateH = 0, kb_xenterH = 0, kb_xcancelH = 0;
int kb_box_max = 0; /* Defined in draw_keyBoard, sets the max width of the string drawn in the box */

/* Character sizes and "SPACE" size */
int kb_char_size = 15, kb_spaceSize = 20, kb_charH_size = 20;

int kb_sel = 27; //Button selected, default is A
int old_sel = 0; //When space is selected this becomes the previous character
int kb_ptr = 0;  //Character selector (string box)

/* For Y or N menu */
int y_alpha = 0;
int n_alpha = 0;
int yn_alpha = 0;
int yn_sel = 0;
