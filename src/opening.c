#include "opening.h"
#include "diagnostic.h"
#include "dos.h"
#include "draw.h"
#include "title_logo.h"
#include "../out/opening_palette.h"

extern const u8 g_Font_MGL_Sample6[];

static u8 g_Buffer[1024];

bool LoadSC5(const c8* filename)
{
	u8 handle = DOS_FOpen(filename, O_RDONLY);
	u16 vramAddr = 0;
	u16 remaining = 27136;
	bool success = FALSE;

	if (handle == 0xFF)
		return FALSE;
	if (DOS_FRead(handle, g_Buffer, 7) != 7 ||
		g_Buffer[0] != 0xFE || g_Buffer[1] != 0 || g_Buffer[2] != 0 ||
		((u16)g_Buffer[3] | ((u16)g_Buffer[4] << 8)) < 27135)
		goto done;

	while (remaining)
	{
		u16 count = remaining > sizeof(g_Buffer) ? sizeof(g_Buffer) : remaining;
		if (DOS_FRead(handle, g_Buffer, count) != count)
			goto done;
		VDP_WriteVRAM_128K(g_Buffer, vramAddr, 0, count);
		vramAddr += count;
		remaining -= count;
	}
	success = TRUE;
done:
	if (DOS_FClose(handle) != DOS_ERR_NONE)
		success = FALSE;
	return success;
}

static void title_text(u16 x, u16 y, const c8* message)
{
	Print_SetColor(OPENING_BLACK, OPENING_BLACK);
	Print_SetPosition(x + 1, y + 1);
	Print_DrawText(message);
	Print_SetColor(OPENING_WHITE, OPENING_BLACK);
	Print_SetPosition(x, y);
	Print_DrawText(message);
}

static void show_title(void)
{
	TitleLogo_Draw(OPENING_BLACK, OPENING_RED);
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetMode(PRINT_MODE_BITMAP_TRANS);
	title_text(41, 20, "Programa de Wilson F. Martins");
	title_text(47, 30, "Remake por Wilson \"B\" Pilon");
	Draw_FillBox(72, 92, 184, 104, OPENING_BLACK, VDP_OP_IMP);
	title_text(77, 94, "A FUGA IMPOSSIVEL");
	Draw_FillBox(35, 181, 222, 195, OPENING_BLACK, VDP_OP_IMP);
	Draw_LineH(35, 222, 181, OPENING_RED, VDP_OP_IMP);
	Draw_LineH(35, 222, 195, OPENING_RED, VDP_OP_IMP);
	title_text(47, 184, "Tecle [ESPACO] para comecar");
	VDP_CommandWait();
}

static void show_unfinished_game(void)
{
	VDP_EnableDisplay(FALSE);
	VDP_SetPalette(g_diagnostic_palette);
	VDP_CommandHMMV(0, 0, 256, 212, 0);
	Print_SetMode(PRINT_MODE_BITMAP);
	Print_SetColor(4, 0);
	Print_SetPosition(56, 90);
	Print_DrawText("JOGO EM DESENVOLVIMENTO");
	Print_SetPosition(100, 106);
	Print_DrawText("ESC: SAIR");
	VDP_CommandWait();
	VDP_EnableDisplay(TRUE);
	while (!Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
}

u8 Opening_Show(void)
{
	VDP_EnableDisplay(FALSE);
	if (!LoadSC5("ALCATRAZ.SC5"))
	{
		VDP_SetPalette(g_diagnostic_palette);
		VDP_CommandHMMV(0, 0, 256, 212, 0x00);
		Print_SetColor(4, 0);
		Print_SetPosition(20, 80);
		Print_DrawText("ERRO AO LER ALCATRAZ.SC5");
		Print_SetPosition(20, 96);
		Print_DrawText("ESC: SAIR");
		VDP_EnableDisplay(TRUE);
		while (!Keyboard_IsKeyPressed(KEY_ESC))
			Halt();
		return 1;
	}
	VDP_SetPalette(g_opening_palette);
	show_title();
	VDP_EnableDisplay(TRUE);
	while (Keyboard_IsKeyPressed(KEY_SPACE) && !Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
	while (!Keyboard_IsKeyPressed(KEY_SPACE) && !Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
	if (Keyboard_IsKeyPressed(KEY_ESC))
		return 0;
	while (Keyboard_IsKeyPressed(KEY_SPACE) && !Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
	if (!Keyboard_IsKeyPressed(KEY_ESC))
		show_unfinished_game();
	return 0;
}
