#include "opening.h"
#include "diagnostic.h"
#include "dos.h"
#include "../out/opening_palette.h"

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
	VDP_EnableDisplay(TRUE);
	while (!Keyboard_IsKeyPressed(KEY_ESC))
		Halt();
	return 0;
}
