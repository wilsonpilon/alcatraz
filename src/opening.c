#include "opening.h"
#include "diagnostic.h"
#include "dos.h"
#include "draw.h"
#include "opening_shots.h"
#include "title_logo.h"
#include "font/font_mgl_std1.h"
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

static void title_line(u16 x, u16 y, const c8* message)
{
	Print_SetPosition(x, y);
	while (*message)
	{
		if (*message == ' ')
			Print_Space();
		else
			Print_DrawChar(*message);
		++message;
	}
}

static void title_text(u16 x, u16 y, const c8* message)
{
	Print_SetColor(OPENING_BLACK, OPENING_BLACK);
	title_line(x + 1, y + 1, message);
	Print_SetColor(OPENING_WHITE, OPENING_BLACK);
	title_line(x, y, message);
}

static bool show_title(Language language)
{
	if (!TitleLogo_Draw(OPENING_BLACK, OPENING_ACCENT))
		return FALSE;
	Print_SetBitmapFont(g_Font_MGL_Std1);
	Print_SetMode(PRINT_MODE_BITMAP_TRANS);
	title_text(language == LANGUAGE_ENGLISH ? 44 : 41, 20,
		language == LANGUAGE_ENGLISH ? "Program by Wilson F. Martins" : "Programa de Wilson F. Martins");
	title_text(language == LANGUAGE_ENGLISH ? 35 : 32, 30,
		language == LANGUAGE_ENGLISH ? "Remake by Wilson \"Barney\" Pilon" : "Remake por Wilson \"Barney\" Pilon");
	Draw_FillBox(language == LANGUAGE_ENGLISH ? 60 : 72, 92,
		language == LANGUAGE_ENGLISH ? 196 : 184, 104, OPENING_BLACK, VDP_OP_IMP);
	title_text(language == LANGUAGE_ENGLISH ? 65 : 77, 94,
		language == LANGUAGE_ENGLISH ? "THE IMPOSSIBLE ESCAPE" : "A FUGA IMPOSSIVEL");
	Draw_FillBox(35, 181, 222, 195, OPENING_BLACK, VDP_OP_IMP);
	Draw_LineH(35, 222, 181, OPENING_ACCENT, VDP_OP_IMP);
	Draw_LineH(35, 222, 195, OPENING_ACCENT, VDP_OP_IMP);
	title_text(language == LANGUAGE_ENGLISH ? 62 : 47, 184,
		language == LANGUAGE_ENGLISH ? "Press [SPACE] to start" : "Tecle [ESPACO] para comecar");
	VDP_CommandWait();
	return TRUE;
}

static void show_unfinished_game(Language language)
{
	VDP_EnableDisplay(FALSE);
	VDP_SetPalette(g_diagnostic_palette);
	VDP_CommandHMMV(0, 0, 256, 212, 0);
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetMode(PRINT_MODE_BITMAP);
	Print_SetColor(4, 0);
	Print_SetPosition(language == LANGUAGE_ENGLISH ? 70 : 56, 90);
	Print_DrawText(language == LANGUAGE_ENGLISH ? "GAME IN DEVELOPMENT" : "JOGO EM DESENVOLVIMENTO");
	Print_SetPosition(100, 106);
	Print_DrawText(language == LANGUAGE_ENGLISH ? "ESC: EXIT" : "ESC: SAIR");
	VDP_CommandWait();
	VDP_EnableDisplay(TRUE);
	while (!Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
}

u8 Opening_Show(Language language)
{
	VDP_EnableDisplay(FALSE);
	if (!LoadSC5("ALCATRAZ.SC5"))
	{
		VDP_SetPalette(g_diagnostic_palette);
		VDP_CommandHMMV(0, 0, 256, 212, 0x00);
		Print_SetColor(4, 0);
		Print_SetPosition(20, 80);
		Print_DrawText(language == LANGUAGE_ENGLISH ? "ERROR READING ALCATRAZ.SC5" : "ERRO AO LER ALCATRAZ.SC5");
		Print_SetPosition(20, 96);
		Print_DrawText(language == LANGUAGE_ENGLISH ? "ESC: EXIT" : "ESC: SAIR");
		VDP_EnableDisplay(TRUE);
		while (!Keyboard_IsKeyPressed(KEY_ESC))
			Halt();
		return 1;
	}
	VDP_SetPalette(g_opening_palette);
	/* Keep clean text bands in the off-screen page at y + 256. */
	VDP_CommandHMMM(0, 20, 0, 276, 256, 19);
	VDP_CommandHMMM(0, 181, 0, 437, 256, 15);
	VDP_CommandWait();
	VDP_EnableDisplay(TRUE);
	if (!show_title(language))
		return 0;
	while (Keyboard_IsKeyPressed(KEY_SPACE) && !Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
	while (!Keyboard_IsKeyPressed(KEY_SPACE) && !Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
	if (Keyboard_IsKeyPressed(KEY_ESC))
		return 0;
	VDP_CommandHMMM(0, 276, 0, 20, 256, 19);
	VDP_CommandHMMM(0, 437, 0, 181, 256, 15);
	VDP_CommandWait();
	if (OpeningShots_Play())
		show_unfinished_game(language);
	return 0;
}
