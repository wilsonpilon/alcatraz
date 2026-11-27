#include "msxgl.h"
#include "disprosoft.h"
#include "font/font_bios_latin.h"

#define TABLE_SIZE 6144
#define PAINT_STACK_SIZE 128

static u8 patterns[TABLE_SIZE];
static u8 colors[TABLE_SIZE];
static u8 paint_x[PAINT_STACK_SIZE];
static u8 paint_y[PAINT_STACK_SIZE];

/* sin(a) * 256, from 0 to PI/2 in 16 steps. */
static const u16 sine[17] = {
	0, 25, 50, 74, 98, 121, 142, 162, 181,
	198, 213, 226, 237, 245, 251, 255, 256
};

static u16 address(u8 x, u8 y)
{
	return ((u16)(y & 0xF8) << 5) + (x & 0xF8) + (y & 7);
}

static void pixel(i16 x, i16 y, u8 color)
{
	u16 pos;
	u8 mask;
	if (x < 0 || x >= 256 || y < 0 || y >= 192)
		return;
	pos = address((u8)x, (u8)y);
	mask = 0x80 >> (x & 7);
	if (color == 1)
		patterns[pos] &= ~mask;
	else
	{
		colors[pos] = (color << 4) | 1;
		patterns[pos] |= mask;
	}
}

static u8 point(u8 x, u8 y)
{
	u16 pos = address(x, y);
	return (patterns[pos] & (0x80 >> (x & 7))) ? colors[pos] >> 4 : colors[pos] & 15;
}

static void line(i16 x0, i16 y0, i16 x1, i16 y1, u8 color)
{
	i16 dx = x1 - x0, dy = y1 - y0;
	i16 sx = 1, sy = 1, err, twice;
	if (dx < 0) { dx = -dx; sx = -1; }
	if (dy < 0) { dy = -dy; sy = -1; }
	err = dx - dy;
	for (;;)
	{
		pixel(x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break;
		twice = err * 2;
		if (twice >= -dy) { err -= dy; x0 += sx; }
		if (twice <= dx) { err += dx; y0 += sy; }
	}
}

static void box(u8 x0, u8 y0, u8 x1, u8 y1, u8 color)
{
	u8 y;
	for (y = y0; y <= y1; ++y)
		line(x0, y, x1, y, color);
}

static i16 sin_step(u8 angle)
{
	angle &= 63;
	if (angle <= 16) return sine[angle];
	if (angle <= 32) return sine[32 - angle];
	if (angle <= 48) return -(i16)sine[angle - 32];
	return -(i16)sine[64 - angle];
}

static i16 scaled(i16 radius, i16 value)
{
	return (radius * value + 128) >> 8;
}

/* CIRCLE angles here are quarter turns, as in this particular listing. */
static void arc(i16 cx, i16 cy, i16 rx, i16 ry, u8 start, u8 end, u8 color)
{
	u8 a = start * 16;
	u8 last = end * 16;
	i16 x0 = cx + scaled(rx, sin_step(a + 16));
	i16 y0 = cy - scaled(ry, sin_step(a));
	for (;;)
	{
		i16 x1, y1;
		if (a == last)
			break;
		a = (a + 1) & 63;
		x1 = cx + scaled(rx, sin_step(a + 16));
		y1 = cy - scaled(ry, sin_step(a));
		line(x0, y0, x1, y1, color);
		x0 = x1;
		y0 = y1;
	}
}

/* Scanline flood fill; a failure is reported rather than drawing a broken logo. */
static bool paint(u8 seed_x, u8 seed_y, u8 color)
{
	u8 count = 0;
	paint_x[count] = seed_x;
	paint_y[count++] = seed_y;
	while (count)
	{
		i16 left, right, x;
		u8 y;
		--count;
		x = paint_x[count];
		y = paint_y[count];
		if (point((u8)x, y) == color)
			continue;
		left = x;
		right = x;
		while (left > 0 && point((u8)(left - 1), y) != color) --left;
		while (right < 255 && point((u8)(right + 1), y) != color) ++right;
		if (left == 0 || right == 255 || y == 0 || y == 191)
			return FALSE;
		for (x = left; x <= right; ++x)
			pixel(x, y, color);
		for (x = left; x <= right; ++x)
		{
			u8 other;
			for (other = y - 1; other <= y + 1; other += 2)
			{
				if (point((u8)x, other) != color &&
					(x == left || point((u8)(x - 1), other) == color))
				{
					if (count == PAINT_STACK_SIZE)
						return FALSE;
					paint_x[count] = (u8)x;
					paint_y[count++] = other;
				}
			}
		}
	}
	return TRUE;
}

static bool draw_logo(void)
{
	box(18,74,33,82,2); line(33,82,39,82,2);
	arc(32,82,7,8,0,1,2); if (!paint(34,77,2)) return FALSE;
	box(18,84,39,92,7); box(18,94,33,102,13);
	line(33,94,39,94,13); arc(32,94,7,8,3,0,13); if (!paint(35,100,13)) return FALSE;
	box(42,74,50,82,2); box(42,84,50,92,7); box(42,94,50,102,13);
	box(67,74,72,82,2); line(72,74,76,74,2); line(64,82,67,82,2);
	arc(69,82,5,8,1,2,2); arc(76,82,4,8,1,2,2);
	if (!paint(66,80,2) || !paint(73,75,2)) return FALSE;
	box(63,84,69,92,7); line(63,92,61,92,7); line(63,84,61,92,7);
	if (!paint(62,91,7)) return FALSE;
	line(69,84,71,84,7); line(71,84,69,92,7); if (!paint(70,85,7)) return FALSE;
	box(53,94,61,102,13); line(61,94,68,94,13);
	arc(61,94,7,8,3,0,13); if (!paint(63,96,13)) return FALSE;
	box(79,74,94,82,2); line(94,82,100,82,2);
	arc(93,82,7,8,0,1,2); if (!paint(95,77,2)) return FALSE;
	box(79,84,87,92,7); line(87,84,100,84,7); line(87,90,96,90,7);
	arc(96,84,4,6,3,0,7); if (!paint(89,86,7)) return FALSE;
	box(79,94,87,102,13); box(103,74,118,82,2); line(118,82,124,82,2);
	arc(117,82,7,8,0,1,2); if (!paint(119,77,2)) return FALSE;
	box(103,84,118,90,7); line(118,84,124,84,7);
	arc(120,84,4,6,3,0,7); line(119,90,124,92,7); line(116,90,116,92,7);
	line(116,92,124,92,7); if (!paint(122,86,7) || !paint(117,91,7)) return FALSE;
	box(116,94,124,102,13); line(127,82,147,82,2);
	arc(137,82,10,8,0,2,2); if (!paint(137,80,2)) return FALSE;
	box(127,84,147,92,7); line(127,94,147,94,13);
	arc(137,94,10,8,2,0,13); if (!paint(137,96,13)) return FALSE;
	pixel(127,93,1); box(164,74,169,82,2); line(169,74,173,74,2);
	line(161,82,164,82,2); arc(166,82,5,8,1,2,2); arc(173,82,4,8,1,2,2);
	if (!paint(163,80,2) || !paint(170,75,2)) return FALSE;
	box(160,84,166,92,7); line(160,92,158,92,7); line(160,84,158,92,7);
	if (!paint(159,91,7)) return FALSE;
	line(166,84,168,84,7); line(168,84,166,92,7); if (!paint(167,85,7)) return FALSE;
	box(150,94,158,102,13); line(158,94,165,94,13);
	arc(158,94,7,8,3,0,13); if (!paint(160,96,13)) return FALSE;
	line(172,82,192,82,2); arc(182,82,10,8,0,2,2);
	if (!paint(182,80,2)) return FALSE;
	box(172,84,192,92,7); line(172,94,192,94,13);
	arc(182,94,10,8,2,0,13); if (!paint(182,96,13)) return FALSE;
	pixel(172,93,1); box(195,74,215,82,2); box(195,84,213,92,7);
	box(195,94,203,102,13); box(218,74,238,82,2); box(218,84,226,92,7);
	line(218,94,226,94,13); line(226,94,226,102,13);
	arc(226,94,8,8,2,3,13); pixel(218,93,1);
	if (!paint(220,96,13)) return FALSE;
	box(226,96,234,102,13);
	return TRUE;
}

bool Disprosoft_Show(void)
{
	u16 i;
	u8 frames;
	static u8 names[256];
	const c8* text = "APRESENTA";
	u16 x = 150;
	VDP_SetMode(VDP_MODE_SCREEN2);
	VDP_EnableDisplay(FALSE);
	VDP_SetDefaultPalette();
	VDP_SetColor(1);
	VDP_DisableSprite();
	for (i = 0; i < TABLE_SIZE; ++i)
	{
		patterns[i] = 0;
		colors[i] = 0xF1;
	}
	if (!draw_logo())
		return FALSE;
	while (*text)
	{
		const u8* glyph = g_Font_BIOS_EU + 4 + ((u16)(u8)*text++ << 3);
		u8 row;
		for (row = 0; row < 8; ++row)
		{
			u8 col;
			for (col = 0; col < 8; ++col)
				if (glyph[row] & (0x80 >> col))
					pixel(x + col, 120 + row, 11);
		}
		x += 8;
	}
	for (i = 0; i < 256; ++i)
		names[i] = (u8)i;
	VDP_WriteVRAM_16K(names, VDP_G2_ADDR_NT, 256);
	VDP_WriteVRAM_16K(names, VDP_G2_ADDR_NT + 256, 256);
	VDP_WriteVRAM_16K(names, VDP_G2_ADDR_NT + 512, 256);
	VDP_WriteVRAM_16K(patterns, VDP_G2_ADDR_PT, TABLE_SIZE);
	VDP_WriteVRAM_16K(colors, VDP_G2_ADDR_CT, TABLE_SIZE);
	VDP_EnableDisplay(TRUE);
	for (frames = 0; frames < 150; ++frames)
		Halt();
	return TRUE;
}
