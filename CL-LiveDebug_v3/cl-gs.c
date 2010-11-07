#include <tamtypes.h>
#include <graph.h>
#include <dma.h>
#include <packet.h>

//#include "fonts/fonts.c"

PACKET pck;

void InitGraph(void)
{
	dma_initialize();
	graph_initialize();
	graph_set_mode(GRAPH_MODE_AUTO, GRAPH_PSM_32, GRAPH_PSM_32);
	graph_set_displaybuffer(0);
	graph_set_drawbuffer(0);
	graph_set_zbuffer(graph_get_width() * graph_get_height() * (graph_get_bpp() >> 3));

	int textureSize = 13000;

	pck.data = memalign(128, textureSize);
	pck.size = (textureSize * (sizeof(u64) * 10));
	pck.count = 0;
}

int FadeImageIn(u32 Addr, int x, int y, int z, int w, int h, int speed)
{
	int i;

	if (speed < 0) { speed = 1; }

	for (i = 0; i < 255; i += speed)
	{
		DrawImage(Addr, x, y, z, w, h, (255-i));
		graph_wait_vsync();
	}
}

int FadeImageOut(u32 Addr, int x, int y, int z, int w, int h, int speed)
{
	int i;

	if (speed < 0) { speed = 1; }

	for (i = 0; i < 255; i += speed)
	{
		DrawImage(Addr, x, y, z, w, h, i);
		graph_wait_vsync();
	}
}

int DrawImage(u32 addr, int x, int y, int z, int w, int h, int fade)
{
	int textureSize;
	//PACKET pck;

	textureSize = w * h;

	pck.count = 0;

	if (textureSize < 13000)
	{
		//pck.data = memalign(128, textureSize);
		//pck.size = (textureSize * (sizeof(u64) * 10));

		pck.count = LoadTexture(x, y, w, h, z, addr, pck, fade);

		if (pck.count > 0)
			dma_channel_send(DMA_CHANNEL_GIF, pck.data, pck.count, 0);

		if (pck.count < 0) { return -1; }

	}
	else
	{
		DrawPhotoSpot(addr, 0, 0, w, h, x, y, z, w, h, 0, 0, 0, fade, 0, 1, 0);
	}

	//free(pck.data);
	//pck.size = 0;
	pck.count = 0;

	return 1;
}

int LoadTexture(int x, int y, int w, int h, int z, u32 addr, PACKET pckt, int fade)
{
	int x2, y2, Left, Top, cnt;
	u8 r, g, b;
	u32 pos = addr;

	Left = 1728 + x;
	Top = 1824 + y;

	cnt = pckt.count;

	if (pckt.data == NULL) { return -2; }

	for (y2 = 0; y2 < h; y2++)
	{
		for (x2 = 0; x2 < w; x2++)
		{
			if ( (pckt.count + (sizeof(u64) * 10)) > pckt.size ) { return -3; }

			r = *(u8*)pos; pos++;
			g = *(u8*)pos; pos++;
			b = *(u8*)pos; pos++;

			if (fade != -1)
			{
				if ((r - fade) > 0) { r -= fade; } else { r = 0; }
				if ((g - fade) > 0) { g -= fade; } else { g = 0; }
				if ((b - fade) > 0) { b -= fade; } else { b = 0; }
			}

			if (r == 0x00) {
			if (g == 0x00) {
			if (b == 0x00) {
				goto skip;
			} } }
				*(u64*)(&pckt.data[cnt]) = GIF_SET_TAG(4, 1, 0, 0, 0, 1);
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = 0x0E;
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = GIF_REG_PRIM;
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = GIF_SET_RGBAQ(r, g, b, 0x80, 0x3F800000);
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = GIF_REG_RGBAQ;
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = GIF_SET_XYZ( (Left+ x2) << 4, (Top + y2) << 4, z);
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = GIF_REG_XYZ2;
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = GIF_SET_XYZ( ((Left + x2) + 1) << 4, ((Top + y2) + 1) << 4, z);
				cnt += sizeof(u64);
				*(u64*)(&pckt.data[cnt]) = GIF_REG_XYZ2;
				cnt += sizeof(u64);
			skip:
		}
	}

	return cnt;
}

void DrawPhotoSpot(u32 ImgAddr, int SpotX, int SpotY, int SpotW, int SpotH, int ImgX, int ImgY, int ImgZ, int ImgW, int ImgH, int Nr, int Ng, int Nb, int Fade, int RGBOrder, int Trans, int Tolerance)
{
	int textureSize = 13000;
	int Left, Top, cnt;
	int chunk = 0;
	int x, y, r, g, b, rgbcount;
	int tmpR, tmpG, tmpB;
	int xAddL, xAddR, yStart;
	//PACKET pck;

	//pck.data = memalign(128, textureSize);
	//pck.size = (textureSize * (sizeof(u64) * 10));
	//pck.count = 0;

	Left = 1728 + ImgX;
	Top = 1824 + ImgY;
	x = 0;
	y = 0;
	r = 0;
	g = 0;
	b = 0;
	rgbcount = 0;

	xAddL = SpotX * 3;
	xAddR = (ImgW - (SpotX + SpotW)) * 3;
	yStart = (ImgW * 3) * SpotY;

	cnt = 0;

	rgbcount += yStart;
	while (y < SpotH)
	{
		rgbcount += xAddL;
		while (x < SpotW)
		{
			r = _lb(ImgAddr + rgbcount + 0);
			g = _lb(ImgAddr + rgbcount + 1);
			b = _lb(ImgAddr + rgbcount + 2);
			rgbcount += 3;

			if ( (Nr != 0) && (x > 4) && (x < (ImgW - 2)) && (y > 4) && (y < (ImgH - 2)) ) { r = Nr; }
			if ( (Ng != 0) && (x > 4) && (x < (ImgW - 2)) && (y > 4) && (y < (ImgH - 2)) ) { g = Ng; }
			if ( (Nb != 0) && (x > 4) && (x < (ImgW - 2)) && (y > 4) && (y < (ImgH - 2)) ) { b = Nb; }

			if (Fade != -1)
			{
				if ((r - Fade) > 0) { r -= Fade; } else { r = 0; }
				if ((g - Fade) > 0) { g -= Fade; } else { g = 0; }
				if ((b - Fade) > 0) { b -= Fade; } else { b = 0; }
			}
			tmpR = r;
			tmpG = g;
			tmpB = b;

			if (RGBOrder == 1)
			{
				r = tmpG;
				g = tmpR;
				b = tmpB;
			}
			else if (RGBOrder == 2)
			{
				r = tmpG;
				g = tmpB;
				b = tmpR;
			}
			else if (RGBOrder == 3)
			{
				r = tmpB;
				g = tmpG;
				b = tmpR;
			}

			if (Trans == 0) { goto ForceRGB; }

			if ( (r - Tolerance) <= 0x00) {
			if ( (g - Tolerance) <= 0x00) {
			if ( (b - Tolerance) <= 0x00) {
				goto skip;
			} } }
				ForceRGB:
				*(u64*)(&pck.data[cnt]) = GIF_SET_TAG(4, 1, 0, 0, 0, 1);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = 0x0E;
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_REG_PRIM;
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_SET_RGBAQ(r, g, b, 0x80, 0x3F800000);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_REG_RGBAQ;
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_SET_XYZ( (Left+ x) << 4, (Top + y) << 4, ImgZ);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_REG_XYZ2;
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_SET_XYZ( ((Left + x) + 1) << 4, ((Top + y) + 1) << 4, ImgZ);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_REG_XYZ2;
				cnt += sizeof(u64);
			skip:

			x++;
			chunk++;
			if (chunk == 13000)
			{
				dma_channel_send(DMA_CHANNEL_GIF, pck.data, cnt, 0);
				chunk = 0;
				cnt = 0;
			}
		}
		x = 0;
		y++;
		rgbcount += xAddR;
	}
	if (chunk < 13000)
	{
		dma_channel_send(DMA_CHANNEL_GIF, pck.data, cnt, 0);
		chunk = 0;
		cnt = 0;
	}

	//free(pck.data);
	//pck.size = 0;
	pck.count = 0;
}

void DrawPhotoSpotMergeBG(u32 ImgAddr, u32 BGAddr, int SpotX, int SpotY, int SpotW, int SpotH, int ImgX, int ImgY, int ImgZ, int ImgW, int ImgH, 
			int bSpotX, int bSpotY, int bSpotW, int bSpotH, int BgW, int BgH)
{
	int textureSize = 13000;
	int Left, Top, cnt;
	int chunk = 0;
	int x, y, r, g, b, rgbcount, bgcount;
	int tmpR, tmpG, tmpB;
	int xAddL, xAddR, yStart, bxAddL, bxAddR, byStart;
	//PACKET pck;

	//pck.data = memalign(128, textureSize);
	//pck.size = (textureSize * (sizeof(u64) * 10));
	//pck.count = 0;

	Left = 1728 + ImgX;
	Top = 1824 + ImgY;
	x = 0;
	y = 0;
	r = 0;
	g = 0;
	b = 0;
	rgbcount = 0;
	bgcount = 0;

	xAddL = SpotX * 3;
	xAddR = (ImgW - (SpotX + SpotW)) * 3;
	yStart = (ImgW * 3) * SpotY;

	bxAddL = bSpotX * 3;
	bxAddR = (BgW - (bSpotX + bSpotW)) * 3;
	byStart = (BgW * 3) * bSpotY;

	cnt = 0;

	rgbcount += yStart;
	bgcount += byStart;
	while (y < SpotH)
	{
		rgbcount += xAddL;
		bgcount += bxAddL;
		while (x < SpotW)
		{
			r = _lb(ImgAddr + rgbcount + 0);
			g = _lb(ImgAddr + rgbcount + 1);
			b = _lb(ImgAddr + rgbcount + 2);

			if (r == 0x00) {
			if (g == 0x00) {
			if (b == 0x00) {
				r = _lb(BGAddr + bgcount + 0);
				g = _lb(BGAddr + bgcount + 1);
				b = _lb(BGAddr + bgcount + 2);
			} } }
				*(u64*)(&pck.data[cnt]) = GIF_SET_TAG(4, 1, 0, 0, 0, 1);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = 0x0E;
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_REG_PRIM;
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_SET_RGBAQ(r, g, b, 0x80, 0x3F800000);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_REG_RGBAQ;
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_SET_XYZ( (Left+ x) << 4, (Top + y) << 4, ImgZ);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_REG_XYZ2;
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_SET_XYZ( ((Left + x) + 1) << 4, ((Top + y) + 1) << 4, ImgZ);
				cnt += sizeof(u64);
				*(u64*)(&pck.data[cnt]) = GIF_REG_XYZ2;
				cnt += sizeof(u64);

			rgbcount += 3;
			bgcount += 3;
			x++;
			chunk++;
			if (chunk == 13000)
			{
				dma_channel_send(DMA_CHANNEL_GIF, pck.data, cnt, 0);
				chunk = 0;
				cnt = 0;
			}
		}
		x = 0;
		y++;
		rgbcount += xAddR;
		bgcount += bxAddR;
	}
	if (chunk < 13000)
	{
		dma_channel_send(DMA_CHANNEL_GIF, pck.data, cnt, 0);
		chunk = 0;
		cnt = 0;
	}

	//free(pck.data);
	//pck.size = 0;
	pck.count = 0;
}

int DrawString(char *strtext, int X, int Y, int Z, int *font, int size, int r, int g, int b, int wraplen, int bgR, int bgG, int bgB, int aln, int thic)
{
	int textureSize = 13000;
	int Left, Top, cnt;
	int NewX, NewY;
	int str;
	int spaceX, spaceY;
	int tr, tg, tb;
	int x, y, c;
	int t, col, row, ix;

	Left = 1728 + X;
	Top = 1824 + Y;

	spaceX = 0;
	spaceY = 0;

	//PACKET pck;

	//pck.data = memalign(128, textureSize);
	//pck.size = (textureSize * (sizeof(u64) * 10));
	//pck.count = 0;

	if (aln == 1) // Center
	{
		for (str = 0; str < strlen(strtext); str++)
		{
			spaceX = ((GetSpacing(strtext[str]) * size) / 2);
			X -= spaceX;
		}
	}
	if (aln == 2) // Right
	{
		for (str = 0; str < strlen(strtext); str++)
		{
			spaceX = (GetSpacing(strtext[str]) * size);
			X -= spaceX;
		}
	}
	// Default to Left

	spaceX = 0;

	for (str = 0; str < strlen(strtext); str++)
	{

		c = strtext[str];

		x = 0;
		y = 0;

		for (t = 1; t < c; t++)
		{
			if (x >= 270) { x = 0; y += 18; }
			x += 18;
		}
		ix = (c * 324) + 1;

		int nx, ny;
		int chunk = 0;

		cnt = 0;

		for (nx = 0; nx < 18; nx++)
		{
			for (ny = 0; ny < 18; ny++)
			{
				if (font[ix] == 1)
				{
					tr = r;
					tg = g;
					tb = b;
					NewX = (Left + X) + (nx * size) + spaceX + thic;
					NewY = (Top + Y) + (ny * size) + spaceY + thic;

				}
				else
				{
					tr = bgR;
					tg = bgG;
					tb = bgB;
				}

				if (r == 0x00) {
				if (g == 0x00) {
				if (b == 0x00) {
					goto skip;
				} } }
					*(u64*)(&pck.data[cnt]) = GIF_SET_TAG(4, 1, 0, 0, 0, 1);
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = 0x0E;
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = GIF_REG_PRIM;
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = GIF_SET_RGBAQ(r, g, b, 0x80, 0x3F800000);
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = GIF_REG_RGBAQ;
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = GIF_SET_XYZ( (NewX) << 4, (NewY) << 4, Z);
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = GIF_REG_XYZ2;
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = GIF_SET_XYZ( ((NewX + size + thic)) << 4, ((NewY + size + thic)) << 4, Z);
					cnt += sizeof(u64);
					*(u64*)(&pck.data[cnt]) = GIF_REG_XYZ2;
					cnt += sizeof(u64);
				skip:

				chunk++;
				ix++;
				if (chunk == 13000)
				{
					dma_channel_send(DMA_CHANNEL_GIF, pck.data, cnt, 0);
					chunk = 0;
					cnt = 0;
				}
			}
		}
		if (chunk <= 13000)
		{
			dma_channel_send(DMA_CHANNEL_GIF, pck.data, cnt, 0);
			chunk = 0;
			cnt = 0;
		}
		spaceX += (GetSpacing(strtext[str]) * size);
		if (spaceX >= wraplen) { spaceX = 0; spaceY += (16 * size); }
	}

	//free(pck.data);
	//pck.size = 0;
	pck.count = 0;
}

int GetSpacing(unsigned char chr)
{
	if (chr < 0x20) { return 0; }
	if (chr == 0x20) { return 10; }
	if (chr == 0x27) { return 6; } // '
	if (chr == 0x28) { return 10; } // (
	if (chr == 0x29) { return 10; } // )
	if (chr == 0x2D) { return 8; }
	if (chr == 0x2E) { return 5; } // .

	if (chr == 0x30) { return 10; } // 0
	if (chr == 0x31) { return 10; } // 1
	if (chr == 0x32) { return 10; } // 2
	if (chr == 0x33) { return 10; } // 3
	if (chr == 0x34) { return 10; } // 4
	if (chr == 0x35) { return 10; } // 5
	if (chr == 0x36) { return 10; } // 6
	if (chr == 0x37) { return 10; } // 7
	if (chr == 0x38) { return 10; } // 8
	if (chr == 0x39) { return 10; } // 9

	if (chr == 0x3A) { return 6; } // :
	if (chr == 0x3B) { return 6; } // ;
	if (chr == 0x3C) { return 10; } // <
	if (chr == 0x3D) { return 10; } // =
	if (chr == 0x3E) { return 10; } // >
	if (chr == 0x3F) { return 10; } // ?

	if (chr == 0x41) { return 13; } // A
	if (chr == 0x42) { return 13; } // B
	if (chr == 0x43) { return 13; } // C
	if (chr == 0x44) { return 13; } // D
	if (chr == 0x45) { return 13; } // E
	if (chr == 0x46) { return 13; } // F
	if (chr == 0x47) { return 13; } // G
	if (chr == 0x48) { return 13; } // H
	if (chr == 0x49) { return 8; } // I
	if (chr == 0x4A) { return 13; } // J
	if (chr == 0x4B) { return 13; } // K
	if (chr == 0x4C) { return 13; } // L
	if (chr == 0x4D) { return 13; } // M
	if (chr == 0x4E) { return 13; } // N
	if (chr == 0x4F) { return 13; } // O
	if (chr == 0x50) { return 13; } // P
	if (chr == 0x51) { return 13; } // Q
	if (chr == 0x52) { return 13; } // R
	if (chr == 0x53) { return 13; } // S
	if (chr == 0x54) { return 13; } // T
	if (chr == 0x55) { return 13; } // U
	if (chr == 0x56) { return 13; } // V
	if (chr == 0x57) { return 16; } // W
	if (chr == 0x58) { return 13; } // X
	if (chr == 0x59) { return 13; } // Y
	if (chr == 0x5A) { return 13; } // Z

	if (chr == 0x5B) { return 10; } // [
	if (chr == 0x5C) { return 10; } // \
	if (chr == 0x5D) { return 10; } // ]
	if (chr == 0x5E) { return 10; } // ^
	if (chr == 0x5F) { return 10; } // _
	if (chr == 0x60) { return 10; } // `

	if (chr == 0x61) { return 10; } // a
	if (chr == 0x62) { return 10; } // b
	if (chr == 0x63) { return 10; } // c
	if (chr == 0x64) { return 10; } // d
	if (chr == 0x65) { return 10; } // e
	if (chr == 0x66) { return 8; } // f
	if (chr == 0x67) { return 10; } // g
	if (chr == 0x68) { return 10; } // h
	if (chr == 0x69) { return 6; } // i
	if (chr == 0x6A) { return 6; } // j
	if (chr == 0x6B) { return 10; } // k
	if (chr == 0x6C) { return 6; } // l
	if (chr == 0x6D) { return 14; } // m
	if (chr == 0x6E) { return 10; } // n
	if (chr == 0x6F) { return 10; } // o
	if (chr == 0x70) { return 10; } // p
	if (chr == 0x71) { return 10; } // q
	if (chr == 0x72) { return 8; } // r
	if (chr == 0x73) { return 10; } // s
	if (chr == 0x74) { return 8; } // t
	if (chr == 0x75) { return 10; } // u
	if (chr == 0x76) { return 10; } // v
	if (chr == 0x77) { return 14; } // w
	if (chr == 0x78) { return 10; } // x
	if (chr == 0x79) { return 10; } // y
	if (chr == 0x7A) { return 10; } // z

	return 18;

}

void DrawBox(int x, int y, int z, int w, int h, int r, int g, int b)
{
	int Left, Top, cnt;
	//PACKET pck;

	Left = 1728;
	Top = 1824;

	//pck.data = memalign(128, 100);
	//pck.size = (100 * (sizeof(u64) * 10));
	//pck.count = 0;

	cnt = 0;

	*(u64*)(&pck.data[cnt]) = GIF_SET_TAG(4, 1, 0, 0, 0, 1);
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = 0x0E;
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = GIF_REG_PRIM;
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = GIF_SET_RGBAQ(r, g, b, 0x80, 0x3F800000);
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = GIF_REG_RGBAQ;
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = GIF_SET_XYZ( (Left + x) << 4, (Top + y) << 4, z);
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = GIF_REG_XYZ2;
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = GIF_SET_XYZ( ((Left + x) + w) << 4, ((Top + y) + h) << 4, z);
	cnt += sizeof(u64);
	*(u64*)(&pck.data[cnt]) = GIF_REG_XYZ2;
	cnt += sizeof(u64);
	dma_channel_send(DMA_CHANNEL_GIF, pck.data, cnt, 0);
	cnt = 0;

	//free(pck.data);
	//pck.size = 0;
	pck.count = 0;
}
