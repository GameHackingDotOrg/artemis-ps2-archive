//---------------------------------------------------------------------------
// File:	g2.h
// Author:	Tony Saveski, t_saveski@yahoo.com
// Notes:	Simple 'High Level' 2D Graphics Library
//---------------------------------------------------------------------------
#ifndef G2_H
#define G2_H

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	 PAL_256_256_32=0
	,PAL_320_256_32
	,PAL_384_256_32
	,PAL_512_256_32
	,PAL_640_256_32

	,NTSC_256_224_32
	,NTSC_320_224_32
	,NTSC_384_224_32
	,NTSC_512_224_32
	,NTSC_640_224_32
} g2_video_mode;

extern int g2_init(g2_video_mode mode);
extern void g2_end(void);

extern uint16 g2_get_max_x(void);
extern uint16 g2_get_max_y(void);

extern void g2_set_color(uint8 r, uint8 g, uint8 b);
extern void g2_set_fill_color(uint8 r, uint8 g, uint8 b);
extern void g2_get_color(uint8 *r, uint8 *g, uint8 *b);
extern void g2_get_fill_color(uint8 *r, uint8 *g, uint8 *b);

extern void g2_put_pixel(int16 x, int16 y);
extern void g2_line(int16 x0, int16 y0, int16 x1, int16 y1);
extern void g2_rect(int16 x0, int16 y0, int16 x1, int16 y1);
extern void g2_fill_rect(int16 x0, int16 y0, int16 x1, int16 y1);
extern void g2_put_image(int16 x, int16 y, int16 w, int16 h, uint32 *data);

extern void g2_set_viewport(uint16 x0, uint16 y0, uint16 x1, uint16 y1);
extern void g2_get_viewport(uint16 *x0, uint16 *y0, uint16 *x1, uint16 *y1);

extern void g2_set_visible_frame(uint8 frame);
extern void g2_set_active_frame(uint8 frame);
extern uint8 g2_get_visible_frame(void);
extern uint8 g2_get_active_frame(void);

extern void g2_wait_vsync(void);
extern void g2_wait_hsync(void);

#ifdef __cplusplus
}
#endif

#endif // G2_H

