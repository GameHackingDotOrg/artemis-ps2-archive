
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
extern void icon_start;
extern void icon_stop;
extern void icon_options;
extern void icon_about;
extern void icon_patches;
extern void icon_mass;
extern void icon_mc;
extern void desc_start;
extern void desc_stop;
extern void desc_options;
extern void desc_about;
extern void desc_patches;
extern void desc_mc0;
extern void desc_mc1;
extern void desc_mass;
extern void menu_bar;
extern void sel_bar1;
extern void sel_bar2;
extern void icon_about_mini;
extern void icon_options_mini;
extern void icon_patches_mini;
extern void font_neuropol;

extern u32 size_background;
extern u32 size_logo;
extern u32 size_logo_ghost;
extern u32 size_icon_start;
extern u32 size_icon_stop;
extern u32 size_icon_options;
extern u32 size_icon_about;
extern u32 size_icon_patches;
extern u32 size_icon_mc;
extern u32 size_icon_mass;
extern u32 size_desc_start;
extern u32 size_desc_stop;
extern u32 size_desc_options;
extern u32 size_desc_about;
extern u32 size_desc_patches;
extern u32 size_desc_mc0;
extern u32 size_desc_mc1;
extern u32 size_desc_mass;
extern u32 size_menu_bar;
extern u32 size_sel_bar1;
extern u32 size_sel_bar2;
extern u32 size_icon_about_mini;
extern u32 size_icon_options_mini;
extern u32 size_icon_patches_mini;
extern u32 size_font_neuropol;

/* include font specific datas */
#include "font_neuropol.h"

//#define DEBUG

struct about_content {
	char *name;
	char *desc;
};

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
int Draw_OptionsMenu(u32 new_pad);
int  Init_AboutMenu(void);
int  Draw_AboutMenu(char *version, struct about_content *about_text);
void Render_GUI(void);
int LoadPatches(int m);
int DrawMenuBar(int X);

GSGLOBAL *gsGlobal;
GSTEXTURE tex_background, tex_logo, tex_logo_ghost;
GSTEXTURE tex_icon_start, tex_icon_options, tex_icon_about, tex_icon_stop, tex_icon_patches, tex_icon_mc, tex_icon_mass;
GSTEXTURE tex_desc_start, tex_desc_options, tex_desc_about, tex_desc_stop, tex_desc_patches, tex_desc_mc0, tex_desc_mc1, tex_desc_mass;
GSTEXTURE tex_icon_about_mini, tex_icon_options_mini, tex_icon_patches_mini;
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
extern int PatchesLoaded;
extern int AutoJoker;
extern int uLEJoker;

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
void drawChar_neuropol(float x, float y, int alpha, u32 width, u32 height, u32 c)
{
	float x1, x2, y1, y2;
	int u1, u2, v1, v2;

	x1 = x;
	x2 = x1 + width;
	y1 = y;
	y2 = y1 + height;

	c -= 32;
	u1 = (c % (tex_font_neuropol.Width/32)) * (tex_font_neuropol.Width/16);
	u2 = u1 + 32;
	v1 = (c - (c % 16)) * 2; /* careful: 6 rows only !!! */
	v2 = v1 + 32;

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
	int l, i;
	float cx;
	int c;

	cx = x;

	l = strlen(string);

	for( i = 0; i < l; i++)	{
		c = (u8)string[i];

		/* catch "\n" */
		if (c == 10) {
			y += fontsize * Y_RATIO;
			cx = x;
		}
		
		/* Draw the string character by character */
		drawChar_neuropol(cx, y, alpha, fontsize, fontsize * Y_RATIO, c);

		/* Uses width informations for neuropol font header file */
		if (c != 10) {
			float f = font_neuropol_width[c-32] * (float)(fontsize / 32.0f);
			cx += (float)(f + fontspacing);
		}
	}
}

/*
 * Calculate and return width in pixels of a string using neuropol font
 */
int getStringWidth_neuropol(const char *string, int fontsize, int fontspacing)
{
	int i, l, c;
	float size;

	l = strlen(string);

	size = 0;

	for( i = 0; i < l; i++) {
		c = (u8)string[i];
		
		float f = font_neuropol_width[c-32] * (float)(fontsize / 32.0f);
		size += (float)(f + fontspacing);
	}

	return (int)size;
}

/*
 * Calculate and return height in pixels of a string using neuropol font
 */
int getStringHeight_neuropol(const char *string, int fontsize)
{
	int l, i, c;
	int height = fontsize * Y_RATIO;

	l = strlen(string);

	for( i = 0; i < l; i++)	{
		c = (u8)string[i];
		if (c > 127) c = 127; /* security check as the font is incomplete */

		/* catch "\n" */
		if (c == 10)
			height += fontsize * Y_RATIO;		
	}
	
	return height;
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
	y = (18 * Y_RATIO) + 50;

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

void draw_image(GSTEXTURE tex, int x, int y, int alpha)
{
	gsKit_prim_sprite_texture(gsGlobal, &tex,
							x, 										/* X1 */
							y,										/* Y1 */
							0,  									/* U1 */
							0,  									/* V1 */
							x + tex.Width, 							/* X2 */
							y + (tex.Height * Y_RATIO), 			/* Y2 */
							tex.Width, 								/* U2 */
							tex.Height,								/* V2 */
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
				tex_background.Filter   = GS_FILTER_LINEAR;
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
				tex_logo_ghost.Filter   = GS_FILTER_LINEAR;
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

	if ((pPng = pngOpenRAW(&logo, size_logo)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_logo.PSM 	  = GS_PSM_CT32;
				tex_logo.Mem 	  = (u32 *)pImgData;
				tex_logo.VramClut = 0;
				tex_logo.Clut	  = NULL;
				tex_logo.Width    = pPng->width;
				tex_logo.Height   = pPng->height;
				tex_logo.Filter   = GS_FILTER_LINEAR;
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
	
	if ((pPng = pngOpenRAW(&icon_start, size_icon_start)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_start.PSM 	  = GS_PSM_CT32;
				tex_icon_start.Mem 	  = (u32 *)pImgData;
				tex_icon_start.VramClut = 0;
				tex_icon_start.Clut	  = NULL;
				tex_icon_start.Width    = pPng->width;
				tex_icon_start.Height   = pPng->height;
				tex_icon_start.Filter   = GS_FILTER_LINEAR;
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
		
	if ((pPng = pngOpenRAW(&icon_stop, size_icon_stop)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_stop.PSM 	  = GS_PSM_CT32;
				tex_icon_stop.Mem 	  = (u32 *)pImgData;
				tex_icon_stop.VramClut = 0;
				tex_icon_stop.Clut	  = NULL;
				tex_icon_stop.Width    = pPng->width;
				tex_icon_stop.Height   = pPng->height;
				tex_icon_stop.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_stop.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_stop.Width, tex_icon_stop.Height, tex_icon_stop.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_stop);
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
				tex_icon_about.Filter   = GS_FILTER_LINEAR;
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

	if ((pPng = pngOpenRAW(&icon_options, size_icon_options)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_options.PSM 	  = GS_PSM_CT32;
				tex_icon_options.Mem 	  = (u32 *)pImgData;
				tex_icon_options.VramClut = 0;
				tex_icon_options.Clut	  = NULL;
				tex_icon_options.Width    = pPng->width;
				tex_icon_options.Height   = pPng->height;
				tex_icon_options.Filter   = GS_FILTER_LINEAR;
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
	
	if ((pPng = pngOpenRAW(&icon_patches, size_icon_patches)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_patches.PSM 	  = GS_PSM_CT32;
				tex_icon_patches.Mem 	  = (u32 *)pImgData;
				tex_icon_patches.VramClut = 0;
				tex_icon_patches.Clut	  = NULL;
				tex_icon_patches.Width    = pPng->width;
				tex_icon_patches.Height   = pPng->height;
				tex_icon_patches.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_patches.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_patches.Width, tex_icon_patches.Height, tex_icon_patches.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_patches);
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
				tex_desc_start.Filter   = GS_FILTER_LINEAR;
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
		
	if ((pPng = pngOpenRAW(&desc_stop, size_desc_stop)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_stop.PSM 	  = GS_PSM_CT32;
				tex_desc_stop.Mem 	  = (u32 *)pImgData;
				tex_desc_stop.VramClut = 0;
				tex_desc_stop.Clut	  = NULL;
				tex_desc_stop.Width    = pPng->width;
				tex_desc_stop.Height   = pPng->height;
				tex_desc_stop.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_stop.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_stop.Width, tex_desc_stop.Height, tex_desc_stop.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_stop);
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
				tex_desc_about.Filter   = GS_FILTER_LINEAR;
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
				tex_desc_options.Filter   = GS_FILTER_LINEAR;
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
	
	if ((pPng = pngOpenRAW(&desc_patches, size_desc_patches)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_patches.PSM 	  = GS_PSM_CT32;
				tex_desc_patches.Mem 	  = (u32 *)pImgData;
				tex_desc_patches.VramClut = 0;
				tex_desc_patches.Clut	  = NULL;
				tex_desc_patches.Width    = pPng->width;
				tex_desc_patches.Height   = pPng->height;
				tex_desc_patches.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_patches.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_patches.Width, tex_desc_patches.Height, tex_desc_patches.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_patches);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	/*
	LoadTexIntoVram(&logo_ghost, size_logo_ghost, tex_logo_ghost);
	LoadTexIntoVram(&logo, size_logo, tex_logo);
	LoadTexIntoVram(&icon_start, size_icon_start, tex_icon_start);
	LoadTexIntoVram(&icon_stop, size_icon_stop, tex_icon_stop);
	LoadTexIntoVram(&icon_about, size_icon_about, tex_icon_about);
	LoadTexIntoVram(&icon_options, size_icon_options, tex_icon_options);
	LoadTexIntoVram(&icon_patches, size_icon_patches, tex_icon_patches);
	LoadTexIntoVram(&desc_start, size_desc_start, tex_desc_start);
	LoadTexIntoVram(&desc_stop, size_desc_stop, tex_desc_stop);
	LoadTexIntoVram(&desc_about, size_desc_about, tex_desc_about);
	LoadTexIntoVram(&desc_options, size_desc_options, tex_desc_options);
	LoadTexIntoVram(&desc_patches, size_desc_patches, tex_desc_patches);
	*/
	
	//LoadTexIntoVram(, size_, tex_);
			
	#ifdef DEBUG
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	#endif
	//SleepThread();
}

void load_patches_Texture(void) {
	pngData *pPng;
	u8		*pImgData;
	
	if ((pPng = pngOpenRAW(&icon_mc, size_icon_mc)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_mc.PSM 	  = GS_PSM_CT32;
				tex_icon_mc.Mem 	  = (u32 *)pImgData;
				tex_icon_mc.VramClut = 0;
				tex_icon_mc.Clut	  = NULL;
				tex_icon_mc.Width    = pPng->width;
				tex_icon_mc.Height   = pPng->height;
				tex_icon_mc.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_mc.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_mc.Width, tex_icon_mc.Height, tex_icon_mc.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_mc);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&icon_mass, size_icon_mass)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_mass.PSM 	  = GS_PSM_CT32;
				tex_icon_mass.Mem 	  = (u32 *)pImgData;
				tex_icon_mass.VramClut = 0;
				tex_icon_mass.Clut	  = NULL;
				tex_icon_mass.Width    = pPng->width;
				tex_icon_mass.Height   = pPng->height;
				tex_icon_mass.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_mass.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_mass.Width, tex_icon_mass.Height, tex_icon_mass.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_mass);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&desc_mc0, size_desc_mc0)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_mc0.PSM 	  = GS_PSM_CT32;
				tex_desc_mc0.Mem 	  = (u32 *)pImgData;
				tex_desc_mc0.VramClut = 0;
				tex_desc_mc0.Clut	  = NULL;
				tex_desc_mc0.Width    = pPng->width;
				tex_desc_mc0.Height   = pPng->height;
				tex_desc_mc0.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_mc0.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_mc0.Width, tex_desc_mc0.Height, tex_desc_mc0.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_mc0);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&desc_mc1, size_desc_mc1)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_mc1.PSM 	  = GS_PSM_CT32;
				tex_desc_mc1.Mem 	  = (u32 *)pImgData;
				tex_desc_mc1.VramClut = 0;
				tex_desc_mc1.Clut	  = NULL;
				tex_desc_mc1.Width    = pPng->width;
				tex_desc_mc1.Height   = pPng->height;
				tex_desc_mc1.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_mc1.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_mc1.Width, tex_desc_mc1.Height, tex_desc_mc1.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_mc1);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&desc_mass, size_desc_mass)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_desc_mass.PSM 	  = GS_PSM_CT32;
				tex_desc_mass.Mem 	  = (u32 *)pImgData;
				tex_desc_mass.VramClut = 0;
				tex_desc_mass.Clut	  = NULL;
				tex_desc_mass.Width    = pPng->width;
				tex_desc_mass.Height   = pPng->height;
				tex_desc_mass.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_desc_mass.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_desc_mass.Width, tex_desc_mass.Height, tex_desc_mass.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_desc_mass);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&icon_patches_mini, size_icon_patches_mini)) > 0) { /* tex size = */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_patches_mini.PSM 	  = GS_PSM_CT32;
				tex_icon_patches_mini.Mem 	  = (u32 *)pImgData;
				tex_icon_patches_mini.VramClut = 0;
				tex_icon_patches_mini.Clut	  = NULL;
				tex_icon_patches_mini.Width    = pPng->width;
				tex_icon_patches_mini.Height   = pPng->height;
				tex_icon_patches_mini.Filter   = GS_FILTER_LINEAR;
				#ifdef DEBUG
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				#endif
				tex_icon_patches_mini.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_icon_patches_mini.Width, tex_icon_patches_mini.Height, tex_icon_patches_mini.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_patches_mini);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
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

	if ((pPng = pngOpenRAW(&icon_about_mini, size_icon_about_mini)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_icon_about_mini.PSM 		= GS_PSM_CT32;
				tex_icon_about_mini.Mem 		= (u32 *)pImgData;
				tex_icon_about_mini.VramClut = 0;
				tex_icon_about_mini.Clut		= NULL;
				tex_icon_about_mini.Width    = pPng->width;
				tex_icon_about_mini.Height   = pPng->height;
				tex_icon_about_mini.Filter   = GS_FILTER_LINEAR;
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
				tex_icon_options_mini.Filter   = GS_FILTER_LINEAR;
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
				tex_menu_bar.Filter   = GS_FILTER_LINEAR;
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
				tex_sel_bar1.Filter   = GS_FILTER_LINEAR;
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
				tex_sel_bar2.Filter   = GS_FILTER_LINEAR;
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
				tex_font_neuropol.Filter   = GS_FILTER_LINEAR;
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
	icon_start_y = SCREEN_HEIGHT + (240 * Y_RATIO);
	icon_stop_y = SCREEN_HEIGHT + (180 * Y_RATIO);
	icon_patches_y = SCREEN_HEIGHT + (120 * Y_RATIO);
	icon_options_y = SCREEN_HEIGHT + (60 * Y_RATIO);	
	icon_about_y = SCREEN_HEIGHT;
	icon_start_alpha = 0;
	icon_stop_alpha = 0;
	icon_about_alpha = 0;
	icon_options_alpha = 0;	
	icon_patches_alpha = 0;	
	desc_start_alpha = 0;
	desc_stop_alpha = 0;
	desc_about_alpha = 0;
	desc_options_alpha = 0;	
	desc_patches_alpha = 0;	
	background_alpha = 128;
	highlight_alpha = 32;
	amount = 6;
	pause_pulse = 0;

}

/*
 * setup GS
 */
void Setup_GS(int gs_vmode)
{
	/* GS Init */
	gsGlobal = gsKit_init_global_custom(
		GS_RENDER_QUEUE_OS_POOLSIZE, //+GS_RENDER_QUEUE_OS_POOLSIZE/2, /* eliminates overflow */
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
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsGlobal->PrimAAEnable = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_OFF;
	gsGlobal->ZBuffering      = GS_SETTING_OFF;
	gsGlobal->PSM = GS_PSM_CT32;
	
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
		
	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);
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
		
	gsKit_queue_reset(gsGlobal->Per_Queue);
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
	int icon_start_fadein_done = 0;
	int icon_stop_fadein_done = 0;
	int icon_options_fadein_done = 0;			
	int icon_about_fadein_done = 0;	
	int icon_patches_fadein_done = 0;		
	int desc_start_fadein_done = 0;
	int desc_stop_fadein_done = 0;
	int desc_options_fadein_done = 0;	
	int desc_about_fadein_done = 0;	
	int desc_patches_fadein_done = 0;
	int icon_start_move_done = 0;
	int icon_stop_move_done = 0;
	int icon_options_move_done = 0;		
	int icon_about_move_done = 0;	
	int icon_patches_move_done = 0;
	
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);

	/* Draw ghost logo */
	draw_logo_ghost(logo_alpha);
				
	/* Draw logo */
	draw_logo(logo_alpha);

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
		
	/* Y coordinate calculation to stop icon moving */
	if (icon_stop_y > min_y) {
		icon_stop_y -= 6;
		if (icon_stop_y <= min_y) {
			icon_stop_y = min_y;
			icon_stop_move_done = 1;
		}
	}
	else
		icon_stop_move_done = 1;

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

	/* Y coordinate calculation to patches icon moving */
	if (icon_patches_y > min_y) {
		icon_patches_y -= 6;
		if (icon_patches_y <= min_y) {
			icon_patches_y = min_y;
			icon_patches_move_done = 1;
		}
	}
	else
		icon_patches_move_done = 1;
		
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
				
	/* Alpha calculation to control icon stop fade-in */
	if (icon_stop_y <= SCREEN_HEIGHT) {	
		if (icon_stop_alpha < 128) {
			icon_stop_alpha += 3;
			if (icon_stop_alpha >= 128) {
				icon_stop_alpha = 128;
				icon_stop_fadein_done = 1;
			}
		}
		else
			icon_stop_fadein_done = 1;
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
	if (icon_about_y <= SCREEN_HEIGHT) {
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
	
	if (icon_patches_y <= SCREEN_HEIGHT) {
		if (icon_patches_alpha < 128) {
			icon_patches_alpha += 3;
			if (icon_patches_alpha >= 128) {
				icon_patches_alpha = 128;
				icon_patches_fadein_done = 1;
			}
		}
		else
			icon_patches_fadein_done = 1;
	}
				
	/* Draw icons */
	draw_image(tex_icon_start, icon_start_x, icon_start_y, icon_start_alpha); //148
	draw_image(tex_icon_stop, icon_stop_x, icon_stop_y - 5, icon_stop_alpha); //242
	draw_image(tex_icon_options, icon_options_x, icon_options_y, icon_options_alpha);	//344
	draw_image(tex_icon_about, icon_about_x, icon_about_y, icon_about_alpha); //442
	draw_image(tex_icon_patches, icon_patches_x, icon_patches_y, icon_patches_alpha); //442
	
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

	/* Alpha calculation to control desc stop fade-in */
	if (desc_start_alpha > 8) {
		if (desc_stop_alpha < 32) {
			desc_stop_alpha += 2;
			if (desc_stop_alpha >= 32) {
				desc_stop_alpha = 32;
				desc_stop_fadein_done = 1;
			}
		}
		else
			desc_stop_fadein_done = 1;
	}	

	/* Alpha calculation to control desc options fade-in */
	if (desc_stop_alpha > 8) {
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
	
	if (desc_about_alpha > 8) {
		if (desc_patches_alpha < 32) {
			desc_patches_alpha += 2;
			if (desc_patches_alpha >= 32) {
				desc_patches_alpha = 32;
				desc_patches_fadein_done = 1;
			}
		}
		else
			desc_patches_fadein_done = 1;
	}
			
	/* Draw descs */
	draw_image(tex_desc_start, desc_start_x, desc_stop_y * Y_RATIO, desc_start_alpha);
	draw_image(tex_desc_stop, desc_stop_x, desc_stop_y * Y_RATIO, desc_stop_alpha);
	draw_image(tex_desc_options, desc_options_x, desc_stop_y * Y_RATIO, desc_options_alpha);	
	draw_image(tex_desc_about, desc_about_x, desc_stop_y * Y_RATIO, desc_about_alpha);
	draw_image(tex_desc_patches, desc_patches_x, desc_stop_y * Y_RATIO, desc_patches_alpha);
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    if ((icon_start_move_done) && (icon_stop_move_done) && (icon_about_move_done) && \
    	(icon_start_fadein_done) && (icon_stop_fadein_done) && (icon_about_fadein_done) && (icon_options_fadein_done) && \
    	(desc_start_fadein_done) && (desc_stop_fadein_done) && (desc_about_fadein_done) && (desc_options_fadein_done) && \
		(icon_patches_move_done) && (icon_patches_fadein_done) && (desc_patches_fadein_done))
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
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);

	/* Draw ghost logo */
	draw_logo_ghost(logo_alpha);
				
	/* Draw logo */
	draw_logo(logo_alpha);
				
	/* Draw icons */		
	draw_image(tex_icon_start, icon_start_x, icon_start_y, icon_start_alpha); //148
	draw_image(tex_icon_stop, icon_stop_x, icon_stop_y - 5, icon_stop_alpha); //242
	draw_image(tex_icon_options, icon_options_x, icon_options_y, icon_options_alpha);	//344
	draw_image(tex_icon_about, icon_about_x, icon_about_y, icon_about_alpha); //442
	draw_image(tex_icon_patches, icon_patches_x, icon_patches_y, icon_patches_alpha); //442
						
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
		highlight_alpha = 128;	
	}
		
	desc_start_alpha = 32;
	desc_stop_alpha = 32;
	desc_options_alpha = 32;	
	desc_about_alpha = 32;
	desc_patches_alpha = 32;
	
	switch (selected_button) {
		case 1:
			desc_start_alpha = highlight_alpha;
			break;
		case 2:
			desc_stop_alpha = highlight_alpha;
			break;
		case 3:
			desc_patches_alpha = highlight_alpha;
			break;
		case 4:
			desc_options_alpha = highlight_alpha;
			break;
		case 5:
			desc_about_alpha = highlight_alpha;
			break;			
	}
	
	/* Draw descs */
	draw_image(tex_desc_start, desc_start_x, desc_stop_y * Y_RATIO, desc_start_alpha);
	draw_image(tex_desc_stop, desc_stop_x, desc_stop_y * Y_RATIO, desc_stop_alpha);
	draw_image(tex_desc_options, desc_options_x, desc_stop_y * Y_RATIO, desc_options_alpha);	
	draw_image(tex_desc_about, desc_about_x, desc_stop_y * Y_RATIO, desc_about_alpha);
	draw_image(tex_desc_patches, desc_patches_x, desc_stop_y * Y_RATIO, desc_patches_alpha);
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
    
    return 1;
}

/*
 * Checks if value si within range, if not it loops
 */
int CheckVal(int val, int min, int max) {
	int low, high;
	low = min; high = max;
	if (low > high) { low = max; high = min; }
	if (low == high) { return -1; }
	
	if (val < low) { return high; }
	if (val > high) { return low; }
	return val;
}
 
/*
 * Draw OptionsMenu
 */
int Draw_OptionsMenu(u32 new_pad)
{
	char *joker = "No", *ule = "No";
	
	if (new_pad & 0x0020) { //Right
		switch (optselbutt) {
			case 1: AutoJoker++; break;
			case 2: uLEJoker++; break;
		}
	}
	if (new_pad & 0x0080) { //Left
		switch (optselbutt) {
			case 1: AutoJoker--; break;
			case 2: uLEJoker--; break;
		}
	}
	if (new_pad & 0x0010) { optselbutt++; } //Up
	if (new_pad & 0x0040) { optselbutt--; } //Down
	
	AutoJoker = CheckVal(AutoJoker, 0, 1);
	uLEJoker = CheckVal(uLEJoker, 0, 1);
	optselbutt = CheckVal(optselbutt, 1, 2);
	
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);
		
	/* Alpha calculation for options menu header */
	if (about_menu_header_alpha < 128) { //Don't mind the name... :P
		about_menu_header_alpha += 2;
		if (about_menu_header_alpha > 128) {
			about_menu_header_alpha = 128;
		}
	}
		
	/* Draw options mini icon */ 
	draw_image(tex_icon_options_mini, 34, 34 * Y_RATIO, about_menu_header_alpha);
	
	/* Draw text */
	drawString_neuropol(61, 33 * Y_RATIO, about_menu_header_alpha, 22, 0, "Options");
	
	DrawMenuBar(menu_bar_x);
	
	if (menu_bar_x == SCREEN_WIDTH) {
	
		if (options_text_alpha < 128) {
			options_text_alpha += 2;		
			if (options_text_alpha >= 128) {
				options_text_alpha = 128;
				highlight_alpha = 32;
			}
		}
	
		drawString_neuropol(40, 80 * Y_RATIO, options_text_alpha, 25, 0, "Auto load pre-set joker on boot?");
		drawString_neuropol(40, 120 * Y_RATIO, options_text_alpha, 25, 0, "Auto load pre-set joker with uLE?");
	
		if (AutoJoker == 0 && options_text_alpha == 128) { joker = "No"; } else if (AutoJoker == 1 && options_text_alpha == 128) { joker = "Yes"; }
		if (uLEJoker == 0 && options_text_alpha == 128) { ule = "No"; } else if (uLEJoker == 1 && options_text_alpha == 128) { ule = "Yes"; }
	
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
	
		joker_alpha = 32;
		ule_alpha = 32;
	
		switch (optselbutt) {
			case 1:
				joker_alpha = highlight_alpha;
				break;
			case 2:
				ule_alpha = highlight_alpha;
				break;
		}
		
		if (options_text_alpha == 128) {
			drawString_neuropol(500, 80 * Y_RATIO, joker_alpha, 25, 0, (char*)joker);
			drawString_neuropol(500, 120 * Y_RATIO, ule_alpha, 25, 0, (char*)ule);
		}
		
	}
	
	if (menu_bar_x < SCREEN_WIDTH) { menu_bar_x += rate; }
	if (menu_bar_x > SCREEN_WIDTH) { menu_bar_x = SCREEN_WIDTH; }
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    return 1;
}

/*
 * Draw PatchesMenu
 */
int Draw_PatchesMenu(u32 new_pad)
{
	int retval = 0;
	
	if (new_pad & 0x0020) { selbutt++; patchret = NULL; } 
	if (new_pad & 0x0080) { selbutt--; patchret = NULL; }

	if (selbutt > 3) { selbutt = 1; }
	if (selbutt < 1) { selbutt = 3; }
	
	if (new_pad & 0x4000) {
		retval = LoadPatches(selbutt - 1);
		if (retval < 0) {
			patchret = "Failed To Load Patch File";
			patch_x = 120;
		} else { 
			patchret = "Successfully Loaded Patch File";
			patch_x = 70;
		}
	}
	
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha);
	
	/* Alpha calculation for patches menu header */
	if (about_menu_header_alpha < 128) { //Don't mind the name... :P
		about_menu_header_alpha += 2;
		if (about_menu_header_alpha > 128) {
			about_menu_header_alpha = 128;
		}
	}
	
	/* Draw patches mini icon */ 
	draw_image(tex_icon_patches_mini, 31, 37 * Y_RATIO, about_menu_header_alpha);
	
	/* Draw text */
	drawString_neuropol(61, 33 * Y_RATIO, about_menu_header_alpha, 22, 0, "Patches");

		/* draw menu delimeter */
	DrawMenuBar(menu_bar_x);
	
	if (menu_bar_x == SCREEN_WIDTH) {
			
		if (patches_image_alpha < 128) {
			patches_image_alpha += 2;		
			if (patches_image_alpha >= 128) {
				patches_image_alpha = 128;
				highlight_alpha = 32;
			}
		}
	
		/* Draw images */
		draw_image(tex_icon_mc, 100, 240 * Y_RATIO, patches_image_alpha);
		draw_image(tex_icon_mc, 250, 240 * Y_RATIO, patches_image_alpha);
		draw_image(tex_icon_mass, 400, 240 * Y_RATIO, patches_image_alpha);
	
		if (patchret != NULL) { drawString_neuropol(patch_x, 400 * Y_RATIO, 128, 30, 0, patchret); }
	
		/* Alpha calculation to control Highlight pulse	*/
		if (patches_image_alpha == 128) {
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
	
		desc_mc0_alpha = 32;
		desc_mc1_alpha = 32;
		desc_mass_alpha = 32;
	
		switch (selbutt) {
			case 1:
				desc_mc0_alpha = highlight_alpha;
				break;
			case 2:
				desc_mc1_alpha = highlight_alpha;
				break;
			case 3:
				desc_mass_alpha = highlight_alpha;
				break;		
		}
		
		/* Draw text descriptions */
		if (patches_image_alpha == 128) {
			draw_image(tex_desc_mc0, 100, 360 * Y_RATIO, desc_mc0_alpha);
			draw_image(tex_desc_mc1, 250, 360 * Y_RATIO, desc_mc1_alpha);
			draw_image(tex_desc_mass, 400, 360 * Y_RATIO, desc_mass_alpha);
		}
		
	}
	if (menu_bar_x < SCREEN_WIDTH) { menu_bar_x += rate; }
	if (menu_bar_x > SCREEN_WIDTH) { menu_bar_x = SCREEN_WIDTH; }
	
	gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
	
    return 0;
}

/*
 * Init AboutMenu
 */
int Init_AboutMenu(void)
{
	about_menu_header_alpha = 0;
	about_menu_thx_alpha = 0;
	about_menu_content_alpha = 0;

	return 1;
}

/*
 * Draw AboutMenu
 */
int Draw_AboutMenu(char *version, struct about_content *about_text)
{
	char ver[16];
	int about_menu_header_fadein_done = 0;
	int about_menu_thx_fadein_done = 0;
	int about_menu_content_fadein_done = 0;

	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);
	
	/* Draw Background */
	draw_background(background_alpha);

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
	draw_image(tex_icon_about_mini, 34, 37 * Y_RATIO, about_menu_header_alpha);
	
	/* Draw text */
	drawString_neuropol(61, 30 * Y_RATIO, about_menu_header_alpha, 29, 2, "About");
	sprintf(ver, "v%s", version);
	drawString_neuropol(550, 39 * Y_RATIO, about_menu_header_alpha, 16, 0, ver);
	
	DrawMenuBar(menu_bar_x);
	if (menu_bar_x < SCREEN_WIDTH) { menu_bar_x += rate; }
	if (menu_bar_x > SCREEN_WIDTH) { menu_bar_x = SCREEN_WIDTH; }
	
	if (menu_bar_x == SCREEN_WIDTH) {
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
		
		centerString_neuropol(99 * Y_RATIO, about_menu_thx_alpha, 25, 1, "Thank you for using CL-LiveDebug!");	
		centerString_neuropol(124 * Y_RATIO, about_menu_thx_alpha, 20, 1, "a PlayStation 2 In-Game Debugger");
	}

	if (about_menu_thx_fadein_done) {
		/* Alpha calculation for about menu content */
		if (about_menu_content_alpha < 128) {
			about_menu_content_alpha += 5;
			if (about_menu_content_alpha >= 128) {
				about_menu_content_alpha = 128;
				about_menu_content_fadein_done = 1;
			}
		}
		else
			about_menu_content_fadein_done = 1;
		
		int y = 180 * Y_RATIO;
		while (about_text->name) {
			drawString_neuropol(34, y, about_menu_content_alpha, 25, 1, about_text->name);
			y += getStringHeight_neuropol(about_text->name, 25);
			drawString_neuropol(34, y, about_menu_content_alpha, 20, 1, about_text->desc);
			y += getStringHeight_neuropol(about_text->desc, 20);
			y += 20 * Y_RATIO;
			about_text++;
		}
	}
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
    
    if (about_menu_content_fadein_done)
    	return 1;

    return 0;
}

int LoadPatches(int m)
{
	int f = 0, i;
	u32 textAddr = 0x00F00000;

	for (i = 0; i < 0x00100000; i += 4)
		*(u32*)(textAddr + i) = 0;

	if (m == 0) { f = fioOpen("mc0:/LDv4/LDv4Patches.txt", O_RDONLY); }
	if (m == 1) { f = fioOpen("mc1:/LDv4/LDv4Patches.txt", O_RDONLY); }
	if (m == 2) { f = fioOpen("mass:/LDv4/LDv4Patches.txt", O_RDONLY); }

	if (f < 0) { return -1; }

	i = fioLseek(f, 0, SEEK_END);
	fioLseek(f, 0, SEEK_SET);

	if (i <= 0) { fioClose(f); return -2; }

	fioRead(f, (u32*)textAddr, i);
	fioClose(f);

	*(u32*)0x00EFFFFC = i;

	PatchesLoaded = 1;

	return 1;
}

/*
 * Draws menu bar from 0 to X
 */
int DrawMenuBar(int X) {
	int cnt = 0;
	
	while (cnt <= X) {
		draw_image(tex_menu_bar, cnt, menu_bar_y * Y_RATIO, menu_bar_a);
		cnt++;
	}
	return 0;
}
