#include <tamtypes.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <png.h>
#include <string.h>

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

/* Functions */
void read_data_fn(png_structp png_ptr, png_bytep buf, png_size_t size);
pngData *pngOpenRAW(u8 *data, int size);
int pngReadImage(pngData *png, u8 *dest);
void pngClose(pngData *png);
void drawChar_neuropol(float x, float y, int alpha, u32 width, u32 height, u32 c);
void drawString_neuropol(u32 x, u32 y, int alpha, int fontsize, int fontspacing, const char *string);
int getStringWidth_neuropol(const char *string, int fontsize, int fontspacing);
int getStringHeight_neuropol(const char *string, int fontsize);
void drawStringRight_neuropol(u32 x, u32 y, int alpha, int fontsize, int fontspacing, const char *string);
void centerString_neuropol(int y, int alpha, int fontsize, int fontspacing, const char *string);
void draw_background(int alpha, int mode);
void draw_logo(int alpha);
void draw_logo_ghost(int alpha);
void draw_image(GSTEXTURE tex, int x, int y, int alpha);
void load_background_Textures(void);
void load_mainmenu_Textures(void);
void load_patches_Texture(void);
void load_submenu_Textures(void);
void load_keyboard_Textures(void);
void load_about_Textures(void);
void load_YorN_Textures(void);
void load_Font(void);

/* Textures */
GSGLOBAL *gsGlobal;
GSTEXTURE tex_background, tex_logo, tex_logo_ghost; //Logo and background
GSTEXTURE tex_icon_start, tex_icon_options, tex_icon_about, tex_icon_stop, tex_icon_patches, tex_icon_mc, tex_icon_mass; //Icons
GSTEXTURE tex_desc_start, tex_desc_options, tex_desc_about, tex_desc_stop, tex_desc_patches, tex_desc_mc0, tex_desc_mc1, tex_desc_mass; //Descriptions
GSTEXTURE tex_icon_about_mini, tex_icon_options_mini, tex_icon_patches_mini; //Mini icons
GSTEXTURE tex_menu_bar, tex_sel_bar1, tex_sel_bar2;
GSTEXTURE tex_font_neuropol; //Font
GSTEXTURE tex_kb_frame, tex_key_sqr, tex_key_rect, tex_key_enter, tex_key_space; //Keyboard
GSTEXTURE tex_yn_frame;

/* Raw textures */
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
extern void kb_frame;
extern void key_sqr;
extern void key_rect;
extern void key_enter;
extern void yn_frame;

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
extern u32 size_kb_frame;
extern u32 size_key_sqr;
extern u32 size_key_rect;
extern u32 size_key_enter;
extern u32 size_yn_frame;
