#include "game_input.h"

static u8 last_row;
static u8 last_mask;

void GameInput_Reset(void)
{
	last_row = 8;
	last_mask = 1;
}

static u8 remember(u8 row, u8 mask, u8 result)
{
	last_row = row;
	last_mask = mask;
	return result;
}

u8 GameInput_Read(void)
{
	static const c8 row2[] = { 0, 0, 0, 0, 0, 0, 'A', 'B' };
	static const c8 row3[] = "CDEFGHIJ";
	static const c8 row4[] = "KLMNOPQR";
	static const c8 row5[] = "STUVWXYZ";
	const c8* letters;
	u8 row, bit, mask, value;

	if (Keyboard_IsKeyPressed(KEY_ESC))
		return GAME_INPUT_ESCAPE;
	if (last_mask)
	{
		if (!(Keyboard_Read(last_row) & last_mask))
			return GAME_INPUT_NONE;
		last_mask = 0;
	}
	value = Keyboard_Read(7);
	if (IS_KEY_PRESSED(value, KEY_RETURN))
		return remember(7, 0x80, GAME_INPUT_ENTER);
	if (IS_KEY_PRESSED(value, KEY_BS))
		return remember(7, 0x20, GAME_INPUT_BACKSPACE);
	value = Keyboard_Read(8);
	if (IS_KEY_PRESSED(value, KEY_SPACE))
		return remember(8, 0x01, ' ');
	if (IS_KEY_PRESSED(value, KEY_UP))
		return remember(8, 0x20, GAME_INPUT_NORTH);
	if (IS_KEY_PRESSED(value, KEY_DOWN))
		return remember(8, 0x40, GAME_INPUT_SOUTH);
	if (IS_KEY_PRESSED(value, KEY_RIGHT))
		return remember(8, 0x80, GAME_INPUT_EAST);
	if (IS_KEY_PRESSED(value, KEY_LEFT))
		return remember(8, 0x10, GAME_INPUT_WEST);
	for (row = 2; row <= 5; ++row)
	{
		value = Keyboard_Read(row);
		letters = row == 2 ? row2 : row == 3 ? row3 : row == 4 ? row4 : row5;
		mask = 1;
		for (bit = 0; bit < 8; ++bit, mask <<= 1)
			if (!(value & mask) && letters[bit])
				return remember(row, mask, letters[bit]);
	}
	return GAME_INPUT_NONE;
}
