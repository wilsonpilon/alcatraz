#include "title_logo.h"
#include "draw.h"

typedef struct { u8 x, y; } Point;
typedef struct { u8 first, count; } Glyph;

/* Original LINE coordinates from docs/abertura.asc, grouped by letter. */
static const Point vertices[] = {
	{44,78},{16,114},{28,114},{36,103},{36,114},{44,114},
	{48,78},{48,114},{72,114},{64,106},{56,106},{56,78},
	{72,78},{72,114},{96,114},{88,106},{80,106},{80,86},{88,86},{96,78},
	{124,78},{96,114},{108,114},{116,103},{116,114},{124,114},
	{124,78},{132,86},{136,86},{136,114},{144,114},{144,86},{148,86},{156,78},
	{156,78},{156,114},{164,114},{164,98},{172,114},{180,114},{172,98},{180,98},{172,78},
	{208,78},{180,114},{192,114},{200,103},{200,114},{208,114},
	{208,78},{240,78},{224,106},{232,106},{240,114},{208,114},{224,86},{216,86}
};

static const Glyph glyphs[] = {
	{0,6}, {6,6}, {12,8}, {20,6}, {26,8}, {34,9}, {43,6}, {49,8}
};

static void draw_glyph(const Glyph* glyph, u8 offset, u8 color)
{
	u8 y, i;
	const Point* points = vertices + glyph->first;
	for (y = 78; y < 114; ++y)
	{
		u8 crossings[10];
		u8 count = 0;
		for (i = 0; i < glyph->count; ++i)
		{
			const Point* a = &points[i];
			const Point* b = &points[(i + 1) % glyph->count];
			if ((a->y <= y && b->y > y) || (b->y <= y && a->y > y))
			{
				i16 x = (i16)a->x + ((i16)b->x - a->x) * ((i16)y - a->y) /
					((i16)b->y - a->y);
				u8 j = count;
				while (j && crossings[j - 1] > x)
				{
					crossings[j] = crossings[j - 1];
					--j;
				}
				crossings[j] = (u8)x;
				++count;
			}
		}
		for (i = 0; i + 1 < count; i += 2)
			Draw_LineH(crossings[i] + offset, crossings[i + 1] + offset,
				y - 26 + offset, color, VDP_OP_IMP);
	}
	for (i = 0; i < glyph->count; ++i)
	{
		const Point* a = &points[i];
		const Point* b = &points[(i + 1) % glyph->count];
		Draw_Line(a->x + offset, a->y - 26 + offset,
			b->x + offset, b->y - 26 + offset, color, VDP_OP_IMP);
	}
}

static bool wait_frames(u8 count)
{
	while (count--)
	{
		if (Keyboard_IsKeyPressed(KEY_ESC))
			return FALSE;
		Halt();
	}
	return !Keyboard_IsKeyPressed(KEY_ESC);
}

bool TitleLogo_Draw(u8 shadow, u8 color)
{
	u8 i;
	if (!wait_frames(45))
		return FALSE;
	for (i = 0; i < sizeof(glyphs) / sizeof(glyphs[0]); ++i)
	{
		draw_glyph(&glyphs[i], 2, shadow);
		draw_glyph(&glyphs[i], 0, color);
		VDP_CommandWait();
		if (!wait_frames(8))
			return FALSE;
	}
	Draw_Line(20, 88, 208, 88, color, VDP_OP_IMP);
	return TRUE;
}
