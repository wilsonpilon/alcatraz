#include "msxgl.h"
#include "dos.h"
#include "dos_mapper.h"
#include "draw.h"
#include "msx-music.h"
#include "psg.h"
#include "font/font_mgl_sample6.h"
#include "../out/build_version.h"
#include "../out/opening_palette.h"

enum { BLACK, NAVY, BLUE, RED, WHITE, GREY };

static const u8 palette[32] = {
	0x00,0, 0x02,0, 0x04,1, 0x50,0, 0x77,7, 0x33,3,
	0x00,0, 0x00,0, 0x00,0, 0x00,0, 0x00,0, 0x00,0,
	0x00,0, 0x00,0, 0x00,0, 0x00,0
};

u8 g_mapper_ok;
u8 g_psg_ok;
u8 g_fm_ok;
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

static void text(u16 x, u16 y, const c8* message)
{
	Print_SetPosition(x, y);
	Print_DrawText(message);
}

static void row(u16 y, const c8* label, bool present)
{
	text(32, y, label);
	Print_SetColor(present ? WHITE : RED, NAVY);
	text(181, y, present ? "OK" : "FALHA");
	Print_SetColor(WHITE, NAVY);
}

void main(void)
{
	DOS_Version version = { 0, 0 };
	u8 machine;

	if (DOS_GetVersion(&version) < 2 || (version.Kernel >> 8) < 2)
	{
		DOS_StringOutput("ALCATRAZ requer MSX-DOS 2.\r\n$");
		DOS_Exit(1);
		return;
	}
	machine = Sys_GetMSXVersion();
	if (machine < 2)
	{
		DOS_StringOutput("ALCATRAZ requer MSX 2+ ou superior.\r\n$");
		DOS_Exit(1);
		return;
	}
	if (DOSMapper_Init())
	{
		u8 segments = DOSMapper_GetVarTable()->NumSeg;
		g_mapper_ok = segments == 0 || segments >= 128;
	}
	g_psg_ok = PSG_DetectDevice(0);
	g_fm_ok = MSXMusic_Detect() != MSXMUSIC_NOTFOUND;

	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_DisableSprite();
	VDP_SetColor(BLACK);
	VDP_SetPalette(palette);
	VDP_CommandHMMV(0, 0, 256, 212, 0x00);
	Draw_FillBox(8, 8, 247, 181, NAVY, VDP_OP_IMP);
	Draw_Box(8, 8, 247, 181, BLUE, VDP_OP_IMP);
	Draw_FillBox(16, 16, 239, 42, RED, VDP_OP_IMP);
	Draw_FillBox(21, 20, 23, 38, WHITE, VDP_OP_IMP);
	Draw_FillBox(29, 20, 31, 38, WHITE, VDP_OP_IMP);
	Draw_FillBox(224, 20, 226, 38, WHITE, VDP_OP_IMP);
	Draw_FillBox(232, 20, 234, 38, WHITE, VDP_OP_IMP);
	Draw_FillBox(16, 157, 239, 172, BLUE, VDP_OP_IMP);
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetColor(WHITE, RED);
	text(87, 21, "ALCATRAZ");
	text(54, 31, "A FUGA IMPOSSIVEL");
	Print_SetColor(WHITE, NAVY);
	text(32, 52, "VERIFICACAO DO SISTEMA");
	row(68, "MSX 2+", TRUE);
	row(79, "MSX-DOS 2", TRUE);
	row(90, "MAPPER 2048 KB", g_mapper_ok);
	row(101, "PSG", g_psg_ok);
	row(112, "FM / MSX-MUSIC", g_fm_ok);
	text(32, 123, "SCC OPCIONAL");
	text(181, 123, "N/T");
	text(32, 142, g_mapper_ok && g_psg_ok && g_fm_ok ? "SISTEMA COMPATIVEL" : "REQUISITOS NAO ATENDIDOS");
	Print_SetColor(WHITE, BLUE);
	text(20, 161, "ENTER: ABRIR  ESC: SAIR  V" ALCATRAZ_VERSION);
	while (!Keyboard_IsKeyPressed(KEY_ESC) &&
		!(g_mapper_ok && g_psg_ok && g_fm_ok && Keyboard_IsKeyPressed(KEY_RETURN)))
		Halt();
	if (Keyboard_IsKeyPressed(KEY_ESC))
	{
		BIOS_Exit(g_mapper_ok && g_psg_ok && g_fm_ok ? 0 : 1);
		return;
	}
	while (Keyboard_IsKeyPressed(KEY_RETURN))
		Halt();
	VDP_EnableDisplay(FALSE);
	if (!LoadSC5("ALCATRAZ.SC5"))
	{
		VDP_SetPalette(palette);
		VDP_CommandHMMV(0, 0, 256, 212, 0x00);
		Print_SetColor(WHITE, BLACK);
		text(20, 80, "ERRO AO LER ALCATRAZ.SC5");
		text(20, 96, "ESC: SAIR");
		VDP_EnableDisplay(TRUE);
		while (!Keyboard_IsKeyPressed(KEY_ESC))
			Halt();
		BIOS_Exit(1);
		return;
	}
	VDP_SetPalette(g_opening_palette);
	VDP_EnableDisplay(TRUE);
	while (!Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
	BIOS_Exit(0);
}
