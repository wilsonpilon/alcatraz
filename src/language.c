#include "language.h"
#include "draw.h"

extern const u8 g_Font_MGL_Sample6[];

enum { BLACK, NAVY, BLUE, WHITE, RED, GREEN, YELLOW, LIGHT_BLUE, GREY };

static const u8 palette[32] = {
	0x00,0, 0x02,0, 0x05,1, 0x77,7, 0x60,0, 0x00,5, 0x70,7, 0x37,4,
	0x33,3, 0x00,0, 0x00,0, 0x00,0, 0x00,0, 0x00,0, 0x00,0, 0x00,0
};

static void text(u16 x, u16 y, const c8* message)
{
	Print_SetPosition(x, y);
	Print_DrawText(message);
}

static void brazil_flag(void)
{
	static const u8 circle_width[19] = {
		0, 4, 6, 7, 8, 8, 9, 9, 9, 9, 9, 9, 9, 8, 8, 7, 6, 4, 0
	};
	u8 row;
	Draw_FillBox(34, 76, 100, 116, GREEN, VDP_OP_IMP);
	for (row = 0; row <= 32; ++row)
	{
		u8 distance = row < 16 ? row : 32 - row;
		u8 width = distance * 27 / 16;
		Draw_LineH(67 - width, 67 + width, 80 + row, YELLOW, VDP_OP_IMP);
	}
	for (row = 0; row < 19; ++row)
		Draw_LineH(67 - circle_width[row], 67 + circle_width[row],
			87 + row, BLUE, VDP_OP_IMP);
	Draw_Line(59, 94, 75, 99, WHITE, VDP_OP_IMP);
}

static void usa_flag(void)
{
	u8 row;
	Draw_FillBox(156, 76, 222, 116, WHITE, VDP_OP_IMP);
	for (row = 0; row < 7; ++row)
		Draw_FillBox(156, 76 + row * 6, 222, 78 + row * 6, RED, VDP_OP_IMP);
	Draw_FillBox(156, 76, 184, 97, BLUE, VDP_OP_IMP);
	for (row = 0; row < 4; ++row)
	{
		u8 x;
		for (x = 160 + (row & 1) * 2; x <= 181; x += 5)
			Draw_FillBox(x, 79 + row * 5, x + 1, 80 + row * 5, WHITE, VDP_OP_IMP);
	}
}

static void highlight(Language language)
{
	u8 left = language == LANGUAGE_PORTUGUESE ? YELLOW : GREY;
	u8 right = language == LANGUAGE_ENGLISH ? YELLOW : GREY;
	Draw_Box(17, 61, 117, 143, left, VDP_OP_IMP);
	Draw_Box(18, 62, 116, 142, left, VDP_OP_IMP);
	Draw_Box(139, 61, 239, 143, right, VDP_OP_IMP);
	Draw_Box(140, 62, 238, 142, right, VDP_OP_IMP);
	VDP_CommandWait();
}

bool Language_Select(Language* language)
{
	Language selected = LANGUAGE_PORTUGUESE;
	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_EnableDisplay(FALSE);
	VDP_SetLineCount(VDP_LINE_212);
	VDP_DisableSprite();
	VDP_SetColor(BLACK);
	VDP_SetPalette(palette);
	VDP_CommandHMMV(0, 0, 256, 212, 0x00);
	Draw_FillBox(6, 7, 249, 203, NAVY, VDP_OP_IMP);
	Draw_Box(6, 7, 249, 203, BLUE, VDP_OP_IMP);
	Draw_FillBox(14, 15, 241, 42, RED, VDP_OP_IMP);
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetMode(PRINT_MODE_BITMAP);
	Print_SetColor(WHITE, RED);
	text(104, 20, "ALCATRAZ");
	text(76, 30, "IDIOMA / LANGUAGE");
	Print_SetColor(WHITE, NAVY);
	text(36, 127, "PORTUGUES");
	text(168, 127, "ENGLISH");
	text(24, 163, "SETAS / ARROWS: ESCOLHER / SELECT");
	text(26, 176, "ENTER: OK    ESC: SAIR / EXIT");
	brazil_flag();
	usa_flag();
	highlight(selected);
	VDP_EnableDisplay(TRUE);
	while (Keyboard_IsKeyPressed(KEY_RETURN) || Keyboard_IsKeyPressed(KEY_LEFT) ||
		Keyboard_IsKeyPressed(KEY_RIGHT))
	{
		if (Keyboard_IsKeyPressed(KEY_ESC))
			return FALSE;
		Halt();
	}
	for (;;)
	{
		if (Keyboard_IsKeyPressed(KEY_ESC))
			return FALSE;
		if (Keyboard_IsKeyPressed(KEY_RETURN))
		{
			while (Keyboard_IsKeyPressed(KEY_RETURN))
			{
				if (Keyboard_IsKeyPressed(KEY_ESC))
					return FALSE;
				Halt();
			}
			*language = selected;
			return TRUE;
		}
		if (Keyboard_IsKeyPressed(KEY_LEFT) || Keyboard_IsKeyPressed(KEY_RIGHT))
		{
			selected = Keyboard_IsKeyPressed(KEY_LEFT) ? LANGUAGE_PORTUGUESE : LANGUAGE_ENGLISH;
			highlight(selected);
			while (Keyboard_IsKeyPressed(KEY_LEFT) || Keyboard_IsKeyPressed(KEY_RIGHT))
			{
				if (Keyboard_IsKeyPressed(KEY_ESC))
					return FALSE;
				Halt();
			}
		}
		Halt();
	}
}
