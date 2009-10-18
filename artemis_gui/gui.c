
#include <tamtypes.h>
#include <kernel.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <loadfile.h>
#include <png.h>
#include <string.h>

extern void background;
extern void logo;
extern void logo_ghost;
extern void gshi_full;
extern void icon_start;
extern void icon_cheats;
extern void icon_options;
extern void icon_about;
extern void desc_start;
extern void desc_cheats;
extern void desc_options;
extern void desc_about;
extern void menu_bar;
extern void sel_bar1;
extern void sel_bar2;
extern void icon_cheats_mini;
extern void icon_about_mini;
extern void icon_options_mini;
extern void font_neuropol;

extern u32 size_background;
extern u32 size_logo;
extern u32 size_logo_ghost;
extern u32 size_gshi_full;
extern u32 size_icon_start;
extern u32 size_icon_cheats;
extern u32 size_icon_options;
extern u32 size_icon_about;
extern u32 size_desc_start;
extern u32 size_desc_cheats;
extern u32 size_desc_options;
extern u32 size_desc_about;
extern u32 size_menu_bar;
extern u32 size_sel_bar1;
extern u32 size_sel_bar2;
extern u32 size_icon_cheats_mini;
extern u32 size_icon_about_mini;
extern u32 size_icon_options_mini;
extern u32 size_font_neuropol;

/* include font specific datas */
#include "font_neuropol.h"

//#define DEBUG

/* fn prototypes */
void Setup_GS(int gs_vmode);
void gfx_set_defaults(void);
void vram_free(void);
void load_background_Textures(void);
void load_mainmenu_Textures(void);
void load_submenu_Textures(void);
void load_Font(void);
void Clear_Screen(void);
int  Draw_INTRO_part1(void);
int  Draw_INTRO_part2(void);
int  Draw_MainMenu(int selected_button, int highlight_pulse);
int  Draw_CheatsMenu(void);
int  Draw_OptionsMenu(void);
int  Init_AboutMenu(void);
int  Draw_AboutMenu(char *version);
void Render_GUI(void);

GSGLOBAL *gsGlobal;
GSTEXTURE tex_background, tex_logo, tex_logo_ghost, tex_gshi_full;
GSTEXTURE tex_icon_start, tex_icon_cheats, tex_icon_options, tex_icon_about;
GSTEXTURE tex_desc_start, tex_desc_cheats, tex_desc_options, tex_desc_about;
GSTEXTURE tex_icon_cheats_mini, tex_icon_about_mini, tex_icon_options_mini;
GSTEXTURE tex_menu_bar, tex_sel_bar1, tex_sel_bar2;
GSTEXTURE tex_font_neuropol;

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
int gshi_alpha;
int gshi_x;
int icon_start_y;
int icon_cheats_y;
int icon_about_y;
int icon_options_y;
int icon_start_alpha;
int icon_cheats_alpha;
int icon_about_alpha;
int icon_options_alpha;
int desc_start_alpha;
int desc_cheats_alpha;
int desc_about_alpha;
int desc_options_alpha;
int highlight_alpha;
int amount;
int pause_pulse;
int stop_pulse_done;

int about_menu_header_alpha;
int about_menu_thx_alpha;
int menu_bar_x;
int icon_about_mini_x;
int str_about_x;
int str_ver_x;

/*
 * PNG handling code: from MyPS2 by ntba2
 */
typedef struct {
	int width;
	int height;
	int bit_depth;
	void *priv;
} pngData;

typedef struct {
	png_structp	png_ptr;
	png_infop	info_ptr, end_info;
	u8 *buf;
	int pos;
	u8 *data;
} pngPrivate;

/*
 * read_data_fn
 */
static void read_data_fn(png_structp png_ptr, png_bytep buf, png_size_t size)
{
	pngPrivate *priv = (pngPrivate*)png_get_io_ptr(png_ptr);

	memcpy(buf, priv->buf + priv->pos, size);
	priv->pos += size;
}

/*
 * pngOpenRAW
 */
pngData *pngOpenRAW(u8 *data, int size)
{
	pngData		*png;
	pngPrivate	*priv;

	if (png_sig_cmp( data, 0, 8 ) != 0)
		return NULL;
		
	if ((png = malloc(sizeof(pngData))) == NULL)
		return NULL;

	memset (png, 0, sizeof(pngData));

	if ((priv = malloc(sizeof(pngPrivate))) == NULL)
		return NULL;

	png->priv = priv;

	priv->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!priv->png_ptr) {
		#ifdef DEBUG
			printf("PNG Read Struct Init Failed\n");		
		#endif	
		free(png);
		return NULL;
	}

	priv->info_ptr = png_create_info_struct(priv->png_ptr);
	if (!priv->info_ptr) {
		#ifdef DEBUG
			printf("PNG Info Struct Init Failed\n");
		#endif
		free(png);
		png_destroy_read_struct(&priv->png_ptr, NULL, NULL);
		return NULL;
	}

	priv->end_info = png_create_info_struct(priv->png_ptr);
	if (!priv->end_info) {
		free(png);
		png_destroy_read_struct(&priv->png_ptr, &priv->info_ptr, NULL);
		return NULL;
	}
	
	priv->buf	= data;
	priv->pos	= 0;

	png_set_read_fn(priv->png_ptr, (png_voidp)priv, read_data_fn);
	png_read_png(priv->png_ptr, priv->info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	png->width	= priv->info_ptr->width;
	png->height	= priv->info_ptr->height;
	png->bit_depth	= priv->info_ptr->channels * 8;

	#ifdef DEBUG
		printf("PNG info: width=%3d ", png->width);
		printf("height=%3d ", png->height);	
		printf("bit depth=%2d ", png->bit_depth);
		printf("color type=%2d\n", priv->png_ptr->color_type);
	#endif
		
	return png;
}

/*
 * pngReadImage
 */
int pngReadImage(pngData *png, u8 *dest)
{
	pngPrivate *priv = png->priv;
	u8 **row_pointers;
	int i, row_ptr;

	int y;

	row_pointers = png_get_rows(priv->png_ptr, priv->info_ptr);
	row_ptr = 0;		

	for (i = 0; i < priv->info_ptr->height; i++) {
		memcpy(dest + row_ptr, row_pointers[i], priv->info_ptr->rowbytes);

		/* need to normalize alpha channel to ps2 range */
		if (priv->info_ptr->channels == 4) {
			for (y = 3; y < priv->info_ptr->rowbytes; y += 4)
				*(dest + row_ptr + y ) /= 2;
		}

		row_ptr += priv->info_ptr->rowbytes;
	}

	return 1;
}

/*
 * pngClose
 */
void pngClose(pngData *png)
{
	pngPrivate *priv = png->priv;

	png_destroy_read_struct(&priv->png_ptr, &priv->info_ptr, &priv->end_info);
	
	if (priv->data)
		free(priv->data);

	free(priv);
	free(png);
}

/*
 * Draw a character with neuropol font
 */
void drawChar_neuropol(u32 x, u32 y, int alpha, u32 width, u32 height, u32 c)
{
	int x1, x2, y1, y2;
	int u1, u2, v1, v2;

	x1 = x;
	x2 = x1 + width;
	y1 = y;
	y2 = y1 + height;

	/* Calculate char coordinates int neuropol texture */
	u1 = (c % (tex_font_neuropol.Width/16)) * (tex_font_neuropol.Width/16);
	u2 = u1 + 16;
	v1 = c - (c % (tex_font_neuropol.Height/8)); /* careful: 8 rows only !!! */
	v2 = v1 + 16;

	/* Draw a char using neuropol texture */
	gsKit_prim_sprite_texture(gsGlobal, &tex_font_neuropol,
							x1, /* X1 */
							y1,	/* Y1 */
							u1, /* U1 */
							v1, /* V1 */
							x2, /* X2 */
							y2,	/* Y2 */
							u2, /* U2 */
							v2, /* V2 */
							0,
							GS_SETREG_RGBAQ(0x00, 0x00, 0x00, alpha, 0x00));
}

/*
 * Draw a string with neuropol font
 */
void drawString_neuropol(u32 x, u32 y, int alpha, int fontsize, int fontspacing, const char *string)
{
	int l, i, cx;
	int c;

	cx = x;

	l = strlen(string);

	for( i = 0; i < l; i++)	{
		c = (u8)string[i];
		if (c > 127) c = 127; /* security check as the font is incomplete */

		/* Draw the string character by character */
		drawChar_neuropol(cx, y, alpha, fontsize, fontsize * Y_RATIO, c);

		/* Uses width informations for neuropol font header file */
		cx += font_neuropol_width[c] + (fontsize-16) + fontspacing;
	}
}

/*
 * Calculate and return width in pixels of a string using neuropol font
 */
int getStringWidth_neuropol(const char *string, int fontsize, int fontspacing)
{
	int i, l, c, size;

	l = strlen(string);

	size = 0;

	for( i = 0; i < l; i++) {
		c = (u8)string[i];
		if (c >= 128) c = 127; /* security check as the font is incomplete */

		size += font_neuropol_width[c] + (fontsize-16) + fontspacing;
	}

	return size;
}

/*
 * Draw a centered string with neuropol font
 */
void centerString_neuropol(int y, int alpha, int fontsize, int fontspacing, const char *string)
{
	int x, str_width;
	
	str_width = getStringWidth_neuropol(string, fontsize, fontspacing);
	
	x = (SCREEN_WIDTH - str_width) / 2;
	
	drawString_neuropol(x, y, alpha, fontsize, fontspacing, string);
}

/*
 * Draw background texture
 */
void draw_background(int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_background,
							0, 						/* X1 */
							0,  					/* Y1 */
							0,  					/* U1 */
							0,  					/* V1 */
							SCREEN_WIDTH,			/* X2 */
							SCREEN_HEIGHT,			/* Y2 */
							tex_background.Width, 	/* U2 */
							tex_background.Height, 	/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw Artemis logo
 */
void draw_logo(int alpha)
{
	int x, y;

	x = (SCREEN_WIDTH - tex_logo.Width) / 2;
	y = 18 * Y_RATIO;

	/* Draw logo */
	gsKit_prim_sprite_texture(gsGlobal, &tex_logo,
							x, 								/* X1 */
							y,								/* Y1 */
							0,  							/* U1 */
							0,  							/* V1 */
							x + tex_logo.Width, 			/* X2 */
							y + (tex_logo.Height * Y_RATIO),/* Y2 */
							tex_logo.Width, 				/* U2 */
							tex_logo.Height,				/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw Artemis ghost logo
 */
void draw_logo_ghost(int alpha)
{
	int x, y;

	x = (SCREEN_WIDTH - tex_logo_ghost.Width) / 2;
	y = -8 * Y_RATIO;

	/* Draw logo */
	gsKit_prim_sprite_texture(gsGlobal, &tex_logo_ghost,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_logo_ghost.Width, 				/* X2 */
							y + (tex_logo_ghost.Height * Y_RATIO),	/* Y2 */
							tex_logo_ghost.Width, 					/* U2 */
							tex_logo_ghost.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw gshi logo
 */
void draw_gshi(int x, int y, int alpha)
{
	/* Draw gshi */
	gsKit_prim_sprite_texture(gsGlobal, &tex_gshi_full,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_gshi_full.Width, 				/* X2 */
							y + (tex_gshi_full.Height * Y_RATIO), 	/* Y2 */
							tex_gshi_full.Width, 					/* U2 */
							tex_gshi_full.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw start icon
 */
void draw_start_icon(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_icon_start,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_icon_start.Width, 				/* X2 */
							y + (tex_icon_start.Height * Y_RATIO), 	/* Y2 */
							tex_icon_start.Width, 					/* U2 */
							tex_icon_start.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw cheats icon
 */
void draw_cheats_icon(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_icon_cheats,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_icon_cheats.Width, 				/* X2 */
							y + (tex_icon_cheats.Height * Y_RATIO), /* Y2 */
							tex_icon_cheats.Width, 					/* U2 */
							tex_icon_cheats.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw about icon
 */
void draw_about_icon(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_icon_about,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_icon_about.Width, 				/* X2 */
							y + (tex_icon_about.Height * Y_RATIO), 	/* Y2 */
							tex_icon_about.Width, 					/* U2 */
							tex_icon_about.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw options icon
 */
void draw_options_icon(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_icon_options,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_icon_options.Width, 			/* X2 */
							y + (tex_icon_options.Height * Y_RATIO),/* Y2 */
							tex_icon_options.Width, 				/* U2 */
							tex_icon_options.Height,				/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw start desc
 */
void draw_start_desc(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_desc_start,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_desc_start.Width, 				/* X2 */
							y + (tex_desc_start.Height * Y_RATIO), 	/* Y2 */
							tex_desc_start.Width, 					/* U2 */
							tex_desc_start.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw cheats desc
 */
void draw_cheats_desc(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_desc_cheats,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_desc_cheats.Width,				/* X2 */
							y + (tex_desc_cheats.Height * Y_RATIO), /* Y2 */
							tex_desc_cheats.Width, 					/* U2 */
							tex_desc_cheats.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw about desc
 */
void draw_about_desc(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_desc_about,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_desc_about.Width, 				/* X2 */
							y + (tex_desc_about.Height * Y_RATIO), 	/* Y2 */
							tex_desc_about.Width, 					/* U2 */
							tex_desc_about.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw options desc
 */
void draw_options_desc(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_desc_options,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_desc_options.Width, 			/* X2 */
							y + (tex_desc_options.Height * Y_RATIO),/* Y2 */
							tex_desc_options.Width, 				/* U2 */
							tex_desc_options.Height,				/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw menu delimeter
 */
void draw_menu_bar(int x, int y, int alpha)
{	
	gsKit_prim_sprite_texture(gsGlobal, &tex_menu_bar,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + SCREEN_WIDTH,		 				/* X2 */
							y + (tex_menu_bar.Height * Y_RATIO),	/* Y2 */
							tex_menu_bar.Width, 					/* U2 */
							tex_menu_bar.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw cheats icon mini
 */
void draw_cheats_icon_mini(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_icon_cheats_mini,
							x, 											/* X1 */
							y,											/* Y1 */
							0,  										/* U1 */
							0,  										/* V1 */
							x + tex_icon_cheats_mini.Width, 			/* X2 */
							y + (tex_icon_cheats_mini.Height * Y_RATIO),/* Y2 */
							tex_icon_cheats_mini.Width, 				/* U2 */
							tex_icon_cheats_mini.Height,				/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw about icon mini
 */
void draw_about_icon_mini(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_icon_about_mini,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex_icon_about_mini.Width, 				/* X2 */
							y + (tex_icon_about_mini.Height * Y_RATIO), 	/* Y2 */
							tex_icon_about_mini.Width, 					/* U2 */
							tex_icon_about_mini.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw options icon mini
 */
void draw_options_icon_mini(int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex_icon_options_mini,
							x, 												/* X1 */
							y,												/* Y1 */
							0,  											/* U1 */
							0,  											/* V1 */
							x + tex_icon_options_mini.Width, 				/* X2 */
							y + (tex_icon_options_mini.Height * Y_RATIO),	/* Y2 */
							tex_icon_options_mini.Width, 					/* U2 */
							tex_icon_options_mini.Height,					/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Clears VRAM
 */
void vram_free(void)
{
	gsKit_vram_clear(gsGlobal);
}

/*
 * Load permanently mainmenu textures into VRAM
 */
void load_background_Textures(void)
{
	pngData *pPng;
	u8		*pImgData;

	//init_scr();
	
	#ifdef DEBUG
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif

	/* gsGlobal->CurrentPointer = vram_pointer; */

	if ((pPng = pngOpenRAW(&background, size_background)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_background.PSM 		= GS_PSM_CT32;
				tex_background.Mem 		= (u32 *)pImgData;
				tex_background.VramClut = 0;
				tex_background.Clut		= NULL;
				tex_background.Width    = pPng->width;
				tex_background.Height   = pPng->height;
				tex_background.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_background.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_background.Width, tex_background.Height, tex_background.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_background);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	#ifdef DEBUG
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif
	//SleepThread();		
}

/*
 * Load permanently mainmenu textures into VRAM
 */
void load_mainmenu_Textures(void)
{
	pngData *pPng;
	u8		*pImgData;

	//init_scr();
	
	#ifdef DEBUG
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif

	/* gsGlobal->CurrentPointer = vram_pointer; */
	
	if ((pPng = pngOpenRAW(&logo_ghost, size_logo_ghost)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_logo_ghost.PSM 	  = GS_PSM_CT32;
				tex_logo_ghost.Mem 	  = (u32 *)pImgData;
				tex_logo_ghost.VramClut = 0;
				tex_logo_ghost.Clut	  = NULL;
				tex_logo_ghost.Width    = pPng->width;
				tex_logo_ghost.Height   = pPng->height;
				tex_logo_ghost.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_logo_ghost.Vram   = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_logo_ghost.Width, tex_logo_ghost.Height, tex_logo_ghost.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_logo_ghost);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	if ((pPng = pngOpenRAW(&logo, size_logo)) > 0) { /* tex size = 0x60000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_logo.PSM 	  = GS_PSM_CT32;
				tex_logo.Mem 	  = (u32 *)pImgData;
				tex_logo.VramClut = 0;
				tex_logo.Clut	  = NULL;
				tex_logo.Width    = pPng->width;
				tex_logo.Height   = pPng->height;
				tex_logo.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_logo.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_logo.Width, tex_logo.Height, tex_logo.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_logo);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&gshi_full, size_gshi_full)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_gshi_full.PSM 	  = GS_PSM_CT32;
				tex_gshi_full.Mem 	  = (u32 *)pImgData;
				tex_gshi_full.VramClut = 0;
				tex_gshi_full.Clut	  = NULL;
				tex_gshi_full.Width    = pPng->width;
				tex_gshi_full.Height   = pPng->height;
				tex_gshi_full.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_gshi_full.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_gshi_full.Width, tex_gshi_full.Height, tex_gshi_full.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_gshi_full);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&icon_start, size_icon_start)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_start.PSM 	  = GS_PSM_CT32;
				tex_icon_start.Mem 	  = (u32 *)pImgData;
				tex_icon_start.VramClut = 0;
				tex_icon_start.Clut	  = NULL;
				tex_icon_start.Width    = pPng->width;
				tex_icon_start.Height   = pPng->height;
				tex_icon_start.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_start.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_start.Width, tex_icon_start.Height, tex_icon_start.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_start);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
		
	if ((pPng = pngOpenRAW(&icon_cheats, size_icon_cheats)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_cheats.PSM 	  = GS_PSM_CT32;
				tex_icon_cheats.Mem 	  = (u32 *)pImgData;
				tex_icon_cheats.VramClut = 0;
				tex_icon_cheats.Clut	  = NULL;
				tex_icon_cheats.Width    = pPng->width;
				tex_icon_cheats.Height   = pPng->height;
				tex_icon_cheats.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_cheats.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_cheats.Width, tex_icon_cheats.Height, tex_icon_cheats.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_cheats);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	if ((pPng = pngOpenRAW(&icon_about, size_icon_about)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_about.PSM 	  = GS_PSM_CT32;
				tex_icon_about.Mem 	  = (u32 *)pImgData;
				tex_icon_about.VramClut = 0;
				tex_icon_about.Clut	  = NULL;
				tex_icon_about.Width    = pPng->width;
				tex_icon_about.Height   = pPng->height;
				tex_icon_about.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_about.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_about.Width, tex_icon_about.Height, tex_icon_about.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_about);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	if ((pPng = pngOpenRAW(&icon_options, size_icon_options)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_options.PSM 	  = GS_PSM_CT32;
				tex_icon_options.Mem 	  = (u32 *)pImgData;
				tex_icon_options.VramClut = 0;
				tex_icon_options.Clut	  = NULL;
				tex_icon_options.Width    = pPng->width;
				tex_icon_options.Height   = pPng->height;
				tex_icon_options.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_options.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_options.Width, tex_icon_options.Height, tex_icon_options.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_options);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
		
	if ((pPng = pngOpenRAW(&desc_start, size_desc_start)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_start.PSM 	  = GS_PSM_CT32;
				tex_desc_start.Mem 	  = (u32 *)pImgData;
				tex_desc_start.VramClut = 0;
				tex_desc_start.Clut	  = NULL;
				tex_desc_start.Width    = pPng->width;
				tex_desc_start.Height   = pPng->height;
				tex_desc_start.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_start.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_start.Width, tex_desc_start.Height, tex_desc_start.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_start);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
		
	if ((pPng = pngOpenRAW(&desc_cheats, size_desc_cheats)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_cheats.PSM 	  = GS_PSM_CT32;
				tex_desc_cheats.Mem 	  = (u32 *)pImgData;
				tex_desc_cheats.VramClut = 0;
				tex_desc_cheats.Clut	  = NULL;
				tex_desc_cheats.Width    = pPng->width;
				tex_desc_cheats.Height   = pPng->height;
				tex_desc_cheats.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_cheats.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_cheats.Width, tex_desc_cheats.Height, tex_desc_cheats.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_cheats);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	if ((pPng = pngOpenRAW(&desc_about, size_desc_about)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_about.PSM 	  = GS_PSM_CT32;
				tex_desc_about.Mem 	  = (u32 *)pImgData;
				tex_desc_about.VramClut = 0;
				tex_desc_about.Clut	  = NULL;
				tex_desc_about.Width    = pPng->width;
				tex_desc_about.Height   = pPng->height;
				tex_desc_about.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_about.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_about.Width, tex_desc_about.Height, tex_desc_about.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_about);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	if ((pPng = pngOpenRAW(&desc_options, size_desc_options)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_options.PSM 	  = GS_PSM_CT32;
				tex_desc_options.Mem 	  = (u32 *)pImgData;
				tex_desc_options.VramClut = 0;
				tex_desc_options.Clut	  = NULL;
				tex_desc_options.Width    = pPng->width;
				tex_desc_options.Height   = pPng->height;
				tex_desc_options.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_options.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_options.Width, tex_desc_options.Height, tex_desc_options.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_options);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
			
	#ifdef DEBUG
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif
	//SleepThread();
}

/*
 * Load permanently menu textures into VRAM
 */
void load_submenu_Textures(void)
{
	pngData *pPng;
	u8		*pImgData;

	//init_scr();
	
	#ifdef DEBUG
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif

	/* gsGlobal->CurrentPointer = vram_pointer; */

	if ((pPng = pngOpenRAW(&icon_cheats_mini, size_icon_cheats_mini)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_cheats_mini.PSM 		= GS_PSM_CT32;
				tex_icon_cheats_mini.Mem 		= (u32 *)pImgData;
				tex_icon_cheats_mini.VramClut = 0;
				tex_icon_cheats_mini.Clut		= NULL;
				tex_icon_cheats_mini.Width    = pPng->width;
				tex_icon_cheats_mini.Height   = pPng->height;
				tex_icon_cheats_mini.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_cheats_mini.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_icon_cheats_mini.Width, tex_icon_cheats_mini.Height, tex_icon_cheats_mini.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_cheats_mini);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	if ((pPng = pngOpenRAW(&icon_about_mini, size_icon_about_mini)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_about_mini.PSM 		= GS_PSM_CT32;
				tex_icon_about_mini.Mem 		= (u32 *)pImgData;
				tex_icon_about_mini.VramClut = 0;
				tex_icon_about_mini.Clut		= NULL;
				tex_icon_about_mini.Width    = pPng->width;
				tex_icon_about_mini.Height   = pPng->height;
				tex_icon_about_mini.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_about_mini.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_icon_about_mini.Width, tex_icon_about_mini.Height, tex_icon_about_mini.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_about_mini);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
		
	if ((pPng = pngOpenRAW(&icon_options_mini, size_icon_options_mini)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_options_mini.PSM 		= GS_PSM_CT32;
				tex_icon_options_mini.Mem 		= (u32 *)pImgData;
				tex_icon_options_mini.VramClut = 0;
				tex_icon_options_mini.Clut		= NULL;
				tex_icon_options_mini.Width    = pPng->width;
				tex_icon_options_mini.Height   = pPng->height;
				tex_icon_options_mini.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_options_mini.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_icon_options_mini.Width, tex_icon_options_mini.Height, tex_icon_options_mini.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_options_mini);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	if ((pPng = pngOpenRAW(&menu_bar, size_menu_bar)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_menu_bar.PSM 		= GS_PSM_CT32;
				tex_menu_bar.Mem 		= (u32 *)pImgData;
				tex_menu_bar.VramClut = 0;
				tex_menu_bar.Clut		= NULL;
				tex_menu_bar.Width    = pPng->width;
				tex_menu_bar.Height   = pPng->height;
				tex_menu_bar.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_menu_bar.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_menu_bar.Width, tex_menu_bar.Height, tex_menu_bar.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_menu_bar);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&sel_bar1, size_sel_bar1)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_sel_bar1.PSM 		= GS_PSM_CT32;
				tex_sel_bar1.Mem 		= (u32 *)pImgData;
				tex_sel_bar1.VramClut = 0;
				tex_sel_bar1.Clut		= NULL;
				tex_sel_bar1.Width    = pPng->width;
				tex_sel_bar1.Height   = pPng->height;
				tex_sel_bar1.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_sel_bar1.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_sel_bar1.Width, tex_sel_bar1.Height, tex_sel_bar1.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_sel_bar1);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}	

	if ((pPng = pngOpenRAW(&sel_bar2, size_sel_bar2)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_sel_bar2.PSM 		= GS_PSM_CT32;
				tex_sel_bar2.Mem 		= (u32 *)pImgData;
				tex_sel_bar2.VramClut = 0;
				tex_sel_bar2.Clut		= NULL;
				tex_sel_bar2.Width    = pPng->width;
				tex_sel_bar2.Height   = pPng->height;
				tex_sel_bar2.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_sel_bar2.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_sel_bar2.Width, tex_sel_bar2.Height, tex_sel_bar2.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_sel_bar2);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}	
			
	#ifdef DEBUG
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif
	//SleepThread();
}

/*
 * Load permanently menu textures into VRAM
 */
void load_Font(void)
{
	pngData *pPng;
	u8		*pImgData;
	
	#ifdef DEBUG
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif

	/* gsGlobal->CurrentPointer = vram_pointer; */

	if ((pPng = pngOpenRAW(&font_neuropol, size_font_neuropol)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_font_neuropol.PSM 		= GS_PSM_CT32;
				tex_font_neuropol.Mem 		= (u32 *)pImgData;
				tex_font_neuropol.VramClut = 0;
				tex_font_neuropol.Clut		= NULL;
				tex_font_neuropol.Width    = pPng->width;
				tex_font_neuropol.Height   = pPng->height;
				tex_font_neuropol.Filter   = GS_FILTER_NEAREST;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_font_neuropol.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_font_neuropol.Width, tex_font_neuropol.Height, tex_font_neuropol.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_font_neuropol);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	#ifdef DEBUG
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif
	//SleepThread();
}

/*
 * Careful !!! Must be called after Load_Texture function, and before drawing anything !
 * Init default for many gfx functions
 */
void gfx_set_defaults(void)
{
	/* Init defaults for GUI */
	logo_alpha = 0;
	gshi_alpha = 0;
	gshi_x = 378;
	icon_start_y = SCREEN_HEIGHT + (180 * Y_RATIO);
	icon_cheats_y = SCREEN_HEIGHT + (120 * Y_RATIO);
	icon_options_y = SCREEN_HEIGHT + (60 * Y_RATIO);	
	icon_about_y = SCREEN_HEIGHT;	
	icon_start_alpha = 0;
	icon_cheats_alpha = 0;
	icon_about_alpha = 0;
	icon_options_alpha = 0;	
	desc_start_alpha = 0;
	desc_cheats_alpha = 0;
	desc_about_alpha = 0;
	desc_options_alpha = 0;	
	stop_pulse_done = 0;
	background_alpha = 128;
	highlight_alpha = 32;
	amount = 6;
	pause_pulse = 0;
	stop_pulse_done = 0;
}

/*
 * setup GS
 */
void Setup_GS(int gs_vmode)
{
	/* GS Init */
	gsGlobal = gsKit_init_global_custom(
		GS_RENDER_QUEUE_OS_POOLSIZE+GS_RENDER_QUEUE_OS_POOLSIZE/2, /* eliminates overflow */
		GS_RENDER_QUEUE_PER_POOLSIZE);

	/* Clear Screen */
	gsKit_clear(gsGlobal, Black);

	/* Mode & screen width/height init */
	gsGlobal->Mode = gs_vmode;
    gsGlobal->Width = SCREEN_WIDTH;
    gsGlobal->Height = SCREEN_HEIGHT;

	/* Screen Position Init */
	gsGlobal->StartX = SCREEN_X;
	gsGlobal->StartY = SCREEN_Y;

	/* Buffer Init */
	gsGlobal->PrimAAEnable = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_OFF;
	gsGlobal->ZBuffering      = GS_SETTING_OFF;
	gsGlobal->PSM = GS_PSM_CT32;
	gsGlobal->PSMZ = GS_PSMZ_16S;

	/* Force Interlace and Field mode */
	gsGlobal->Interlace = GS_INTERLACED;
	gsGlobal->Field     = GS_FIELD;

	/* DMAC Init */
	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);

	/* Screen Init */
	gsKit_init_screen(gsGlobal);
	gsKit_clear(gsGlobal, Black);
	gsKit_mode_switch(gsGlobal, GS_ONESHOT);
}

/*
 * render GUI
 */
void Render_GUI(void)
{
	/* Flips Framebuffers on VSync */
	gsKit_sync_flip(gsGlobal);

	/* Normal User Draw Queue "Execution" (Kicks Oneshot and Persistent Queues) */
	gsKit_queue_exec(gsGlobal);
}

/*
 * clear screen
 */
void Clear_Screen(void)
{
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);
}

/*
 * Draw intro scene part 1
 * return 1 if intro is finished
 * otherwise return 0
 */
int Draw_INTRO_part1(void)
{
	int intro_done = 0;
	int logo_fadein_done = 0;

	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Alpha calculation to control logo fade-in */
	if (logo_alpha < 128) {
		logo_alpha += 2;
		if (logo_alpha >= 128) {
			logo_alpha = 128;
			logo_fadein_done = 1;
		}
	}
	else
		logo_fadein_done = 1;

	/* Draw Background */
	draw_background(background_alpha);

	/* Draw ghost logo */
	draw_logo_ghost(logo_alpha);
				
	/* Draw logo */
	draw_logo(logo_alpha);
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    if (logo_fadein_done)
    	intro_done = 1;

    /* Return 1 if intro have finished playing */
    if (intro_done) {
    	#ifdef DEBUG
	    	printf("Intro part 1 is done\n");
	    #endif

	    return 1;
    }
    return 0;
}

/*
 * Draw intro scene part 2
 * return 1 if intro is finished
 * otherwise return 0
 */
int Draw_INTRO_part2(void)
{
	int intro_done = 0;
	int gshi_fadein_done = 0;
	int icon_start_fadein_done = 0;
	int icon_cheats_fadein_done = 0;
	int icon_options_fadein_done = 0;			
	int icon_about_fadein_done = 0;		
	int desc_start_fadein_done = 0;
	int desc_cheats_fadein_done = 0;
	int desc_options_fadein_done = 0;	
	int desc_about_fadein_done = 0;		
	int ghsi_logo_move_done = 0;
	int icon_start_move_done = 0;
	int icon_cheats_move_done = 0;
	int icon_options_move_done = 0;		
	int icon_about_move_done = 0;		

	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);

	/* Draw ghost logo */
	draw_logo_ghost(logo_alpha);
				
	/* Draw logo */
	draw_logo(logo_alpha);

	/* Alpha calculation to control gshi fade-in */
	if (gshi_alpha < 128) {
		gshi_alpha += 2;
		if (gshi_alpha >= 128) {
			gshi_alpha = 128;
			gshi_fadein_done = 1;
		}
	}
	else
		gshi_fadein_done = 1;
	
	/* X coordinate calculation to gshi moving */
	if (gshi_x > 226) {
		gshi_x -= 4;
		if (gshi_x <= 226) {
			gshi_x = 226;
			ghsi_logo_move_done = 1;
		}
	}
	else
		ghsi_logo_move_done = 1;
		
	/* Draw gshi */
	draw_gshi(gshi_x, (106 * Y_RATIO), gshi_alpha);

	int min_y = 300 * Y_RATIO;
	/* Y coordinate calculation to start icon moving */
	if (icon_start_y > min_y) {
		icon_start_y -= 6;
		if (icon_start_y <= min_y) {
			icon_start_y = min_y;
			icon_start_move_done = 1;
		}
	}
	else
		icon_start_move_done = 1;
		
	/* Y coordinate calculation to cheats icon moving */
	if (icon_cheats_y > min_y) {
		icon_cheats_y -= 6;
		if (icon_cheats_y <= min_y) {
			icon_cheats_y = min_y;
			icon_cheats_move_done = 1;
		}
	}
	else
		icon_cheats_move_done = 1;

	/* Y coordinate calculation to options icon moving */
	if (icon_options_y > min_y) {
		icon_options_y -= 6;
		if (icon_options_y <= min_y) {
			icon_options_y = min_y;
			icon_options_move_done = 1;
		}
	}
	else
		icon_options_move_done = 1;		
		
	/* Y coordinate calculation to about icon moving */
	if (icon_about_y > min_y) {
		icon_about_y -= 6;
		if (icon_about_y <= min_y) {
			icon_about_y = min_y;
			icon_about_move_done = 1;
		}
	}
	else
		icon_about_move_done = 1;

	/* Alpha calculation to control icon start fade-in */
	if (icon_start_y <= SCREEN_HEIGHT) {
		if (icon_start_alpha < 128) {
			icon_start_alpha += 3;
			if (icon_start_alpha >= 128) {
				icon_start_alpha = 128;
				icon_start_fadein_done = 1;
			}
		}
		else
			icon_start_fadein_done = 1;
	}
				
	/* Alpha calculation to control icon cheats fade-in */
	if (icon_cheats_y <= SCREEN_HEIGHT) {	
		if (icon_cheats_alpha < 128) {
			icon_cheats_alpha += 3;
			if (icon_cheats_alpha >= 128) {
				icon_cheats_alpha = 128;
				icon_cheats_fadein_done = 1;
			}
		}
		else
			icon_cheats_fadein_done = 1;
	}

	/* Alpha calculation to control icon options fade-in */
	if (icon_options_y <= SCREEN_HEIGHT) {	
		if (icon_options_alpha < 128) {
			icon_options_alpha += 3;
			if (icon_options_alpha >= 128) {
				icon_options_alpha = 128;
				icon_options_fadein_done = 1;
			}
		}
		else
			icon_options_fadein_done = 1;
	}
						
	/* Alpha calculation to control icon about fade-in */
	if (icon_start_y <= SCREEN_HEIGHT) {
		if (icon_about_alpha < 128) {
			icon_about_alpha += 3;
			if (icon_about_alpha >= 128) {
				icon_about_alpha = 128;
				icon_about_fadein_done = 1;
			}
		}
		else
			icon_about_fadein_done = 1;
	}
				
	/* Draw icons */
	draw_start_icon(148, icon_start_y, icon_start_alpha);
	draw_cheats_icon(242, icon_cheats_y, icon_cheats_alpha);
	draw_options_icon(344, icon_options_y, icon_options_alpha);	
	draw_about_icon(442, icon_about_y, icon_about_alpha);	
			
	/* Alpha calculation to control desc start fade-in */
	if (icon_start_fadein_done) {
		if (desc_start_alpha < 32) {
			desc_start_alpha += 2;
			if (desc_start_alpha >= 32) {
				desc_start_alpha = 32;
				desc_start_fadein_done = 1;
			}
		}
		else
			desc_start_fadein_done = 1;
	}	

	/* Alpha calculation to control desc cheats fade-in */
	if (desc_start_alpha > 8) {
		if (desc_cheats_alpha < 32) {
			desc_cheats_alpha += 2;
			if (desc_cheats_alpha >= 32) {
				desc_cheats_alpha = 32;
				desc_cheats_fadein_done = 1;
			}
		}
		else
			desc_cheats_fadein_done = 1;
	}	

	/* Alpha calculation to control desc options fade-in */
	if (desc_cheats_alpha > 8) {
		if (desc_options_alpha < 32) {
			desc_options_alpha += 2;
			if (desc_options_alpha >= 32) {
				desc_options_alpha = 32;
				desc_options_fadein_done = 1;
			}
		}
		else
			desc_options_fadein_done = 1;
	}	
	
	/* Alpha calculation to control desc about fade-in */
	if (desc_options_alpha > 8) {
		if (desc_about_alpha < 32) {
			desc_about_alpha += 2;
			if (desc_about_alpha >= 32) {
				desc_about_alpha = 32;
				desc_about_fadein_done = 1;
			}
		}
		else
			desc_about_fadein_done = 1;
	}	
			
	/* Draw descs */
	draw_start_desc(143, 357 * Y_RATIO, desc_start_alpha);
	draw_cheats_desc(240, 357 * Y_RATIO, desc_cheats_alpha);
	draw_options_desc(335, 357 * Y_RATIO, desc_options_alpha);	
	draw_about_desc(436, 354 * Y_RATIO, desc_about_alpha);
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    if ((gshi_fadein_done) && (ghsi_logo_move_done) && \
    	(icon_start_move_done) && (icon_cheats_move_done) && (icon_about_move_done) && \
    	(icon_start_fadein_done) && (icon_cheats_fadein_done) && (icon_about_fadein_done) && (icon_options_fadein_done) && \
    	(desc_start_fadein_done) && (desc_cheats_fadein_done) && (desc_about_fadein_done) && (desc_options_fadein_done))
    	intro_done = 1;

    /* Return 1 if intro have finished playing */
    if (intro_done) {
    	#ifdef DEBUG
	    	printf("Intro part 2 is done\n");
	    #endif

	    return 1;
    }
    return 0;
}

/*
 * Draw MainMenu
 */
int Draw_MainMenu(int selected_button, int highlight_pulse)
{
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);

	/* Draw ghost logo */
	draw_logo_ghost(logo_alpha);
				
	/* Draw logo */
	draw_logo(logo_alpha);

	/* Draw gshi */
	draw_gshi(gshi_x, (106 * Y_RATIO), gshi_alpha);
				
	/* Draw icons */
	draw_start_icon(148, icon_start_y, icon_start_alpha);
	draw_cheats_icon(242, icon_cheats_y, icon_cheats_alpha);
	draw_options_icon(344, icon_options_y, icon_options_alpha);	
	draw_about_icon(442, icon_about_y, icon_about_alpha);	
						
	/* Alpha calculation to control Highlight pulse	*/
	if (highlight_pulse) {
		highlight_alpha += amount;
		if (highlight_alpha > 128) {
			highlight_alpha = 128;
			pause_pulse++;
			if (pause_pulse >= 12) {
				amount = -3;
				pause_pulse = 0;
			}
		} else if (highlight_alpha < 32) {
			amount = 3;
			highlight_alpha = 32;
		}
	}
	else {
		stop_pulse_done = 0;
		if (highlight_alpha <= 128) {
			amount = 3;
			highlight_alpha += amount;	
			if (highlight_alpha >= 128) {
				highlight_alpha = 128;
				stop_pulse_done = 1;
			} 	
		}
	}
		
	desc_start_alpha = 32;
	desc_cheats_alpha = 32;
	desc_options_alpha = 32;	
	desc_about_alpha = 32;
	
	switch (selected_button) {
		case 1:
			desc_start_alpha = highlight_alpha;
			break;
		case 2:
			desc_cheats_alpha = highlight_alpha;
			break;
		case 3:
			desc_options_alpha = highlight_alpha;
			break;
		case 4:
			desc_about_alpha = highlight_alpha;
			break;			
	}
	
	/* Draw descs */
	draw_start_desc(143, 357 * Y_RATIO, desc_start_alpha);
	draw_cheats_desc(240, 357 * Y_RATIO, desc_cheats_alpha);
	draw_options_desc(335, 357 * Y_RATIO, desc_options_alpha);	
	draw_about_desc(436, 354 * Y_RATIO, desc_about_alpha);
		
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

	if (highlight_pulse)
		return 1;
    else if (stop_pulse_done)
    	return 1;
    
    return 0;
}

/*
 * Draw CheatsMenu
 */
int Draw_CheatsMenu(void)
{
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);
		
	/* Draw cheats mini icon */ 
	draw_cheats_icon_mini(34, 37 * Y_RATIO, 128);
	
	/* Draw text */
	drawString_neuropol(61, 33 * Y_RATIO, 128, 22, 0, "Choose cheats");
		
	/* draw menu delimeter */			
	//draw_menu_bar(128);
				
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    return 1;
}

/*
 * Draw OptionsMenu
 */
int Draw_OptionsMenu(void)
{
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);
		
	/* Draw options mini icon */ 
	draw_options_icon_mini(34, 37 * Y_RATIO, 128);
	
	/* Draw text */
	drawString_neuropol(61, 33 * Y_RATIO, 128, 22, 0, "Options");
		
	/* draw menu delimeter */			
	//draw_menu_bar(128);
				
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    return 1;
}

/*
 * Init AboutMenu
 */
int Init_AboutMenu(void)
{
	about_menu_header_alpha = 0;
	about_menu_thx_alpha = 0;	
	menu_bar_x = SCREEN_WIDTH;
	icon_about_mini_x = SCREEN_WIDTH + 40;
	str_about_x = SCREEN_WIDTH + 67;
	str_ver_x = SCREEN_WIDTH + 556;
	
	return 1;
}

/*
 * Draw AboutMenu
 */
int Draw_AboutMenu(char *version)
{
	char ver[16];
	int menu_bar_move_done = 0;
	int about_menu_header_fadein_done = 0;
	int about_menu_thx_fadein_done = 0;
	
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);
	
	/* X coordinate calculation to aboutmenu text moving */
	if (menu_bar_x > 0) {
		icon_about_mini_x -= 12;
		str_about_x -= 12;
		str_ver_x -= 12;
		menu_bar_x -= 12;		
		if (menu_bar_x <= 0)
			menu_bar_move_done = 1;
	}
	else
		menu_bar_move_done = 1;

	/* Alpha calculation for about menu header */
	if (about_menu_header_alpha < 128) {
		about_menu_header_alpha += 2;
		if (about_menu_header_alpha >= 128) {
			about_menu_header_alpha = 128;
			about_menu_header_fadein_done = 1;
		}
	}
	else
		about_menu_header_fadein_done = 1;
					
	/* Draw about mini icon */ 
	draw_about_icon_mini(icon_about_mini_x, 37 * Y_RATIO, about_menu_header_alpha);
	
	/* Draw text */
	drawString_neuropol(str_about_x, 33 * Y_RATIO, about_menu_header_alpha, 22, 0, "About");
	sprintf(ver, "v%s", version);
	drawString_neuropol(str_ver_x, 37 * Y_RATIO, about_menu_header_alpha, 16, 0, ver);

	/* draw menu delimeter */			
	draw_menu_bar(menu_bar_x, 60 * Y_RATIO, about_menu_header_alpha);
		
	if (menu_bar_move_done) {
		/* Alpha calculation for about menu header */
		if (about_menu_thx_alpha < 128) {
			about_menu_thx_alpha += 5;
			if (about_menu_thx_alpha >= 128) {
				about_menu_thx_alpha = 128;
				about_menu_thx_fadein_done = 1;
			}
		}
		else
			about_menu_thx_fadein_done = 1;
		
		centerString_neuropol(102 * Y_RATIO, about_menu_thx_alpha, 17, 0, "Thank you for using Artemis!");	
		centerString_neuropol(123 * Y_RATIO, about_menu_thx_alpha,15, 0, "a PlayStation 2 Hacking System");
	}
		
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    return 1;
}
