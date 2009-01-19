#include <tamtypes.h>
#include "gif.h"
#include "ps2.h"
#include "dma.h"
#include "gs.h"

// static u16	gs_origin_x;	// used for mapping Primitive to Window coordinate systems
// static u16	gs_origin_y;

//---------------------------------------------------------------------------
DECLARE_GS_PACKET(gs_dma_buf,50);

void g2_put_pixel(int16 x, int16 y, u8 r, u8 g, u8 b)
{
// 	x += gs_origin_x;
// 	y += gs_origin_y;

	BEGIN_GS_PACKET(gs_dma_buf);
	GIF_TAG_AD(gs_dma_buf, 4, 1, 0, 0, 0);

	GIF_DATA_AD(gs_dma_buf, prim,
		GS_PRIM(PRIM_POINT, 0, 0, 0, 0, 0, 0, 0, 0));

	GIF_DATA_AD(gs_dma_buf, rgbaq,
		GS_RGBAQ(r, g, b, 0x80, 0));

	// The XYZ coordinates are actually floating point numbers between
	// 0 and 4096 represented as unsigned integers where the lowest order
	// four bits are the fractional point. That's why all coordinates are
	// shifted left 4 bits.
	GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x<<4, y<<4, 0));

	SEND_GS_PACKET(gs_dma_buf);
}

void g2_fill_rect(s16 x0, s16 y0, s16 x1, s16 y1, u8 fill_r, u8 fill_g, u8 fill_b)
{
// 	s16 x0 = 30;
// 	s16 y0 = 30;
// 	s16 x1 = 100;
// 	s16 y1 = 100;
// 	u8 fill_r = 0xFF, fill_g = 0xFF,  fill_b = 0xFF;
// 	x0 += gs_origin_x;
// 	y0 += gs_origin_y;
// 	x1 += gs_origin_x;
// 	y1 += gs_origin_y;

	BEGIN_GS_PACKET(gs_dma_buf);
	GIF_TAG_AD(gs_dma_buf, 4, 1, 0, 0, 0);

	GIF_DATA_AD(gs_dma_buf, prim,
		GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));

	GIF_DATA_AD(gs_dma_buf, rgbaq,
		GS_RGBAQ(fill_r, fill_g, fill_b, 0x80, 0));

	GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x0<<4, y0<<4, 0));

	// It looks like the default operation for the SPRITE primitive is to
	// not draw the right and bottom 'lines' of the rectangle refined by
	// the parameters. Add +1 to change this.
	GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2((x1+1)<<4, (y1+1)<<4, 0));

	SEND_GS_PACKET(gs_dma_buf);
}
