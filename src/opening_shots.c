#include "opening_shots.h"
#include "draw.h"
#include "msx-music.h"
#include "psg.h"
#include "../out/opening_palette.h"

static void draw_impact(u16 x, u16 y)
{
	Draw_Line(x - 9, y - 4, x + 6, y + 6, OPENING_BLACK, VDP_OP_IMP);
	Draw_Line(x - 8, y + 5, x + 7, y - 7, OPENING_BLACK, VDP_OP_IMP);
	Draw_Line(x - 11, y + 2, x + 9, y, OPENING_BLACK, VDP_OP_IMP);
	Draw_Line(x - 1, y - 8, x + 3, y + 9, OPENING_BLACK, VDP_OP_IMP);
	Draw_Line(x - 9, y - 5, x - 3, y - 2, OPENING_GLASS, VDP_OP_IMP);
	Draw_Line(x + 3, y + 1, x + 6, y + 5, OPENING_GLASS, VDP_OP_IMP);
	Draw_Line(x - 8, y + 4, x - 2, y + 1, OPENING_GLASS, VDP_OP_IMP);
	Draw_Line(x + 2, y - 2, x + 7, y - 8, OPENING_GLASS, VDP_OP_IMP);
	Draw_Line(x - 11, y + 1, x - 3, y, OPENING_GLASS, VDP_OP_IMP);
	Draw_Line(x + 3, y - 1, x + 9, y - 1, OPENING_GLASS, VDP_OP_IMP);
	Draw_Line(x - 2, y - 8, x - 1, y - 3, OPENING_GLASS, VDP_OP_IMP);
	Draw_Line(x + 1, y + 3, x + 2, y + 9, OPENING_GLASS, VDP_OP_IMP);
	Draw_Line(x - 7, y - 4, x - 9, y - 7, OPENING_WHITE, VDP_OP_IMP);
	Draw_Line(x + 5, y + 4, x + 9, y + 3, OPENING_WHITE, VDP_OP_IMP);
	Draw_FillBox(x - 2, y - 2, x + 2, y + 2, OPENING_BLACK, VDP_OP_IMP);
	Draw_Point(x - 2, y - 2, OPENING_WHITE, VDP_OP_IMP);
	Draw_Point(x + 3, y + 1, OPENING_WHITE, VDP_OP_IMP);
	VDP_CommandWait();
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

bool OpeningShots_Play(void)
{
	static const u8 patch[] = { 0x03, 0x01, 0x1C, 0x07, 0xF5, 0xF6, 0x0F, 0x07 };
	u8 mixer = PSG_GetRegister(PSG_REG_MIXER);
	u8 noise = PSG_GetRegister(PSG_REG_NOISE);
	u8 volume = PSG_GetRegister(PSG_REG_AMP_A);
	u8 i;
	bool completed = TRUE;

	PSG_SetRegister(PSG_REG_AMP_A, 0);
	PSG_SetRegister(PSG_REG_NOISE, 12);
	PSG_SetRegister(PSG_REG_MIXER, (mixer | PSG_F_TA) & ~PSG_F_NA);
	for (i = 0; i < sizeof(patch); ++i)
		MSXMusic_SetRegister(MSXMUSIC_REG_USER_0 + i, patch[i]);
	MSXMusic_SetRegister(MSXMUSIC_REG_INST_1, 0x03);
	MSXMusic_SetRegister(MSXMUSIC_REG_FREQ_7, 0x70);
	MSXMusic_SetRegister(MSXMUSIC_REG_CTRL_7, 0x04);
	MSXMusic_SetRegister(MSXMUSIC_REG_FREQ_8, 0xA0);
	MSXMusic_SetRegister(MSXMUSIC_REG_CTRL_8, 0x05);
	MSXMusic_SetRegister(MSXMUSIC_REG_FREQ_9, 0x50);
	MSXMusic_SetRegister(MSXMUSIC_REG_CTRL_9, 0x05);
	MSXMusic_SetRegister(MSXMUSIC_REG_RHYTHM_VOL_0, 0x04);
	MSXMusic_SetRegister(MSXMUSIC_REG_RHYTHM_VOL_1, 0xF4);
	MSXMusic_SetRegister(MSXMUSIC_REG_RHYTHM_VOL_2, 0xFF);
	MSXMusic_SetRegister(MSXMUSIC_REG_RHYTHM, 0x20);

	for (i = 0; i < 10; ++i)
	{
		draw_impact(20 + (u16)i * 24, 48 + (u16)i * 10);
		PSG_SetRegister(PSG_REG_NOISE, 8);
		PSG_SetRegister(PSG_REG_AMP_A, 11);
		MSXMusic_SetRegister(MSXMUSIC_REG_FREQ_1, 0x80 + (i & 1) * 16);
		MSXMusic_SetRegister(MSXMUSIC_REG_CTRL_1, 0x18);
		MSXMusic_SetRegister(MSXMUSIC_REG_RHYTHM, 0x38);
		if (!wait_frames(2)) { completed = FALSE; break; }
		PSG_SetRegister(PSG_REG_NOISE, 18);
		PSG_SetRegister(PSG_REG_AMP_A, 7);
		MSXMusic_SetRegister(MSXMUSIC_REG_CTRL_1, 0x08);
		MSXMusic_SetRegister(MSXMUSIC_REG_RHYTHM, 0x20);
		if (!wait_frames(2)) { completed = FALSE; break; }
		PSG_SetRegister(PSG_REG_AMP_A, 3);
		if (!wait_frames(2)) { completed = FALSE; break; }
		PSG_SetRegister(PSG_REG_AMP_A, 0);
		if (!wait_frames(3)) { completed = FALSE; break; }
	}
	PSG_SetRegister(PSG_REG_AMP_A, 0);
	MSXMusic_SetRegister(MSXMUSIC_REG_CTRL_1, 0x00);
	MSXMusic_SetRegister(MSXMUSIC_REG_RHYTHM, 0x00);
	PSG_SetRegister(PSG_REG_NOISE, noise);
	PSG_SetRegister(PSG_REG_MIXER, mixer);
	PSG_SetRegister(PSG_REG_AMP_A, volume);
	if (completed)
		completed = wait_frames(VDP_GetFrequency() == VDP_FREQ_50HZ ? 100 : 120);
	return completed;
}
