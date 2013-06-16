#include "textures.h"
#include "font_neuropol.h"

extern int DEBUG;

/* Keyboard */
extern int kb_yoff;
extern int kb_yrate;
extern int kb_yspace;
extern int kb_xoff;
extern int kb_xrate;
extern int kb_xspace;
extern int kb_frame_y;
extern int kb_frame_x;
extern int kb_frameH_y;
extern int kb_frameH_x;
extern int kb_yoffH;
extern int kb_yrateH;
extern int kb_yenterH;
extern int kb_xoffH;
extern int kb_xrateH;
extern int kb_xenterH;
extern int kb_xcancelH;

/* GS */
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int SCREEN_X;
extern int SCREEN_Y;
extern float Y_RATIO;

/* This contains all the functions relating to reading, uploading, and drawing textures */

/*
 * read_data_fn
 */
void read_data_fn(png_structp png_ptr, png_bytep buf, png_size_t size) {
	pngPrivate *priv = (pngPrivate*)png_get_io_ptr(png_ptr);

	memcpy(buf, priv->buf + priv->pos, size);
	priv->pos += size;
}

/*
 * pngOpenRAW
 */
pngData *pngOpenRAW(u8 *data, int size) {
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
		if (DEBUG)
			printf("PNG Read Struct Init Failed\n");		
		free(png);
		return NULL;
	}

	priv->info_ptr = png_create_info_struct(priv->png_ptr);
	if (!priv->info_ptr) {
		if (DEBUG)
			printf("PNG Info Struct Init Failed\n");
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

	if (DEBUG == 2) {
		printf("PNG info: width=%3d ", png->width);
		printf("height=%3d ", png->height);	
		printf("height=%3d ", png->height);	
		printf("bit depth=%2d ", png->bit_depth);
		printf("color type=%2d\n", priv->png_ptr->color_type);
	}
		
	return png;
}

/*
 * pngReadImage
 */
int pngReadImage(pngData *png, u8 *dest) {
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
void pngClose(pngData *png) {
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
void drawChar_neuropol(float x, float y, int alpha, u32 width, u32 height, u32 c) {
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
void drawString_neuropol(u32 x, u32 y, int alpha, int fontsize, int fontspacing, const char *string) {
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
int getStringWidth_neuropol(const char *string, int fontsize, int fontspacing) {
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
int getStringHeight_neuropol(const char *string, int fontsize) {
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
 * Draw a right aligned string with neuropol font
 */
void drawStringRight_neuropol(u32 x, u32 y, int alpha, int fontsize, int fontspacing, const char *string) {
	int l, i;
	float cx;
	int c;

	cx = x - getStringWidth_neuropol(string, fontsize, fontspacing);
	
	l = strlen(string);

	for(i = 0; i < l; i++)	{
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
 * Draw a centered string with neuropol font
 */
void centerString_neuropol(int y, int alpha, int fontsize, int fontspacing, const char *string) {
	int x, str_width;
	
	str_width = getStringWidth_neuropol(string, fontsize, fontspacing);
	
	x = (SCREEN_WIDTH - str_width) / 2;
	
	drawString_neuropol(x, y, alpha, fontsize, fontspacing, string);
}

/*
 * Draw background texture
 */
void draw_background(int alpha, int mode) {

	GSTEXTURE backg;
	switch (mode) {
		case 0:
			backg = tex_background;
			break;
		case 1:
			backg = tex_yn_frame;
			break;
	}

	gsKit_prim_sprite_texture(gsGlobal, &backg,
							0, 						/* X1 */
							0,  					/* Y1 */
							0,  					/* U1 */
							0,  					/* V1 */
							SCREEN_WIDTH,			/* X2 */
							SCREEN_HEIGHT,			/* Y2 */
							backg.Width, 	/* U2 */
							backg.Height, 	/* V2 */
							0,
							GS_SETREG_RGBAQ(0x80, 0x80, 0x80, alpha, 0x00));
}

/*
 * Draw CL-LiveDebug logo
 */
void draw_logo(int alpha) {
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
 * Draw CL-LiveDebug ghost logo
 */
void draw_logo_ghost(int alpha) {
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
 * Draws tex
 */
void draw_image(GSTEXTURE tex, int x, int y, int alpha) {
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
 * Load permanently mainmenu textures into VRAM
 */
void load_background_Textures(void) {
	pngData *pPng;
	u8		*pImgData;

	//init_scr();
	
	if (DEBUG == 2) {
		printf("Background textures:\n");
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}
	
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_background.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_background.Width, tex_background.Height, tex_background.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_background);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	/*if ((pPng = pngOpenRAW(&logo_ghost, size_logo_ghost)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_logo_ghost.PSM 	  = GS_PSM_CT32;
				tex_logo_ghost.Mem 	  = (u32 *)pImgData;
				tex_logo_ghost.VramClut = 0;
				tex_logo_ghost.Clut	  = NULL;
				tex_logo_ghost.Width    = pPng->width;
				tex_logo_ghost.Height   = pPng->height;
				tex_logo_ghost.Filter   = GS_FILTER_LINEAR;
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_logo_ghost.Vram   = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_logo_ghost.Width, tex_logo_ghost.Height, tex_logo_ghost.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_logo_ghost);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}*/

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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_logo.Vram 	  = gsKit_vram_alloc(gsGlobal,
	 						  			gsKit_texture_size(tex_logo.Width, tex_logo.Height, tex_logo.PSM),
	 						  			GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_logo);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

	if ((pPng = pngOpenRAW(&menu_bar, size_menu_bar)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_menu_bar.PSM 		= GS_PSM_CT32;
				tex_menu_bar.Mem 		= (u32 *)pImgData;
				tex_menu_bar.VramClut = 0;
				tex_menu_bar.Clut		= NULL;
				tex_menu_bar.Width    = pPng->width;
				tex_menu_bar.Height   = pPng->height;
				tex_menu_bar.Filter   = GS_FILTER_LINEAR;
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_menu_bar.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_menu_bar.Width, tex_menu_bar.Height, tex_menu_bar.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_menu_bar);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if (DEBUG == 2) {
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}
	//SleepThread();		
}

/*
 * Load permanently mainmenu textures into VRAM
 */
void load_mainmenu_Textures(void) {
	pngData *pPng;
	u8		*pImgData;
	
	if (DEBUG == 2) {
		printf("Mainmenu textures:\n");
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}

	/* gsGlobal->CurrentPointer = vram_pointer; */
	
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
	//LoadTexIntoVram(, size_, tex_);
	*/
			
	if (DEBUG == 2) {
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}
	//SleepThread();
}

void load_patches_Texture(void) {
	pngData *pPng;
	u8		*pImgData;
	
	if (DEBUG == 2)
		printf("Patches textures:\n");
	
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
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
void load_submenu_Textures(void) {
	pngData *pPng;
	u8		*pImgData;

	//init_scr();
	
	if (DEBUG == 2) {
		printf("Submenu/options textures:\n");
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}

	/* gsGlobal->CurrentPointer = vram_pointer; */
		
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_icon_options_mini.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_icon_options_mini.Width, tex_icon_options_mini.Height, tex_icon_options_mini.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_options_mini);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
			
	if (DEBUG == 2) {
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}
	//SleepThread();
}

void load_YorN_Textures(void) {
	pngData *pPng;
	u8		*pImgData;

	//init_scr();
	
	if (DEBUG == 2) {
		printf("YorN textures:\n");
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}

	/* gsGlobal->CurrentPointer = vram_pointer; */

	if ((pPng = pngOpenRAW(&yn_frame, size_yn_frame)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_yn_frame.PSM 		= GS_PSM_CT32;
				tex_yn_frame.Mem 		= (u32 *)pImgData;
				tex_yn_frame.VramClut = 0;
				tex_yn_frame.Clut		= NULL;
				tex_yn_frame.Width    = pPng->width;
				tex_yn_frame.Height   = pPng->height;
				tex_yn_frame.Filter   = GS_FILTER_LINEAR;
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_yn_frame.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_yn_frame.Width, tex_yn_frame.Height, tex_yn_frame.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_yn_frame);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}

}

void load_keyboard_Textures(void) {
	pngData *pPng;
	u8		*pImgData;

	//init_scr();
	
	if (DEBUG == 2) {
		printf("Keyboard textures:\n");
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}

	/* gsGlobal->CurrentPointer = vram_pointer; */

	if ((pPng = pngOpenRAW(&kb_frame, size_kb_frame)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_kb_frame.PSM 		= GS_PSM_CT32;
				tex_kb_frame.Mem 		= (u32 *)pImgData;
				tex_kb_frame.VramClut = 0;
				tex_kb_frame.Clut		= NULL;
				tex_kb_frame.Width    = pPng->width;
				tex_kb_frame.Height   = pPng->height;
				tex_kb_frame.Filter   = GS_FILTER_LINEAR;
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_kb_frame.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_kb_frame.Width, tex_kb_frame.Height, tex_kb_frame.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_kb_frame);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&key_sqr, size_key_sqr)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_key_sqr.PSM 		= GS_PSM_CT32;
				tex_key_sqr.Mem 		= (u32 *)pImgData;
				tex_key_sqr.VramClut = 0;
				tex_key_sqr.Clut		= NULL;
				tex_key_sqr.Width    = pPng->width;
				tex_key_sqr.Height   = pPng->height;
				tex_key_sqr.Filter   = GS_FILTER_LINEAR;
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_key_sqr.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_key_sqr.Width, tex_key_sqr.Height, tex_key_sqr.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_key_sqr);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&key_rect, size_key_rect)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_key_rect.PSM 		= GS_PSM_CT32;
				tex_key_rect.Mem 		= (u32 *)pImgData;
				tex_key_rect.VramClut = 0;
				tex_key_rect.Clut		= NULL;
				tex_key_rect.Width    = pPng->width;
				tex_key_rect.Height   = pPng->height;
				tex_key_rect.Filter   = GS_FILTER_LINEAR;
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_key_rect.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_key_rect.Width, tex_key_rect.Height, tex_key_rect.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_key_rect);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&key_enter, size_key_enter)) > 0) { /* tex size = 0x140000 */
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_key_enter.PSM 		= GS_PSM_CT32;
				tex_key_enter.Mem 		= (u32 *)pImgData;
				tex_key_enter.VramClut = 0;
				tex_key_enter.Clut		= NULL;
				tex_key_enter.Width    = pPng->width;
				tex_key_enter.Height   = pPng->height;
				tex_key_enter.Filter   = GS_FILTER_LINEAR;
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_key_enter.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_key_enter.Width, tex_key_enter.Height, tex_key_enter.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_key_enter);
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_sel_bar2.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_sel_bar2.Width, tex_sel_bar2.Height, tex_sel_bar2.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_sel_bar2);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}	
			
	if (DEBUG == 2) {
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}
	//SleepThread();
}

void load_about_Textures(void) {
	
	pngData *pPng;
	u8		*pImgData;
	
	if (DEBUG == 2)
		printf("About textures:\n");
	
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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_icon_about_mini.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_icon_about_mini.Width, tex_icon_about_mini.Height, tex_icon_about_mini.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_icon_about_mini);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
	if ((pPng = pngOpenRAW(&menu_bar, size_menu_bar)) > 0) {
		if ((pImgData = malloc(pPng->width * pPng->height * (pPng->bit_depth / 8))) > 0) {
			if (pngReadImage( pPng, pImgData ) != -1) {
				tex_menu_bar.PSM 		= GS_PSM_CT32;
				tex_menu_bar.Mem 		= (u32 *)pImgData;
				tex_menu_bar.VramClut = 0;
				tex_menu_bar.Clut		= NULL;
				tex_menu_bar.Width    = pPng->width;
				tex_menu_bar.Height   = pPng->height;
				tex_menu_bar.Filter   = GS_FILTER_LINEAR;
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_menu_bar.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_menu_bar.Width, tex_menu_bar.Height, tex_menu_bar.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_menu_bar);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	
}

/*
 * Load permanently menu textures into VRAM
 */
void load_Font(void) {
	pngData *pPng;
	u8		*pImgData;
	
	if (DEBUG == 2) {
		printf("Font textures:\n");
		printf("1st VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}

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
				if (DEBUG == 2) {
					printf("VRAM Pointer = %08x  ", gsGlobal->CurrentPointer);
					printf("texture size = %x\n", gsKit_texture_size(pPng->width, pPng->height, GS_PSM_CT32));
				}
				tex_font_neuropol.Vram 	= gsKit_vram_alloc(gsGlobal,
			 						  		gsKit_texture_size(tex_font_neuropol.Width, tex_font_neuropol.Height, tex_font_neuropol.PSM),
			 						  		GSKIT_ALLOC_USERBUFFER);
				gsKit_texture_upload(gsGlobal, &tex_font_neuropol);
			}
			pngClose(pPng);
			free(pImgData);
		}
	}
	if (DEBUG == 2) {
		printf("Load_GUI last VRAM Pointer = %08x  \n", gsGlobal->CurrentPointer);
	}
	//SleepThread();
}
