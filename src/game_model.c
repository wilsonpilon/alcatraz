#include "game_model.h"

static bool matches(const c8* value, const c8* expected)
{
	while (*value && *value == *expected)
	{
		++value;
		++expected;
	}
	return *value == *expected;
}

static bool equals(const c8* value, const c8* pt, const c8* en, Language language)
{
	return matches(value, language == LANGUAGE_ENGLISH ? en : pt);
}

void GameModel_Init(GameState* state)
{
	u8 i;
	for (i = 0; i < ITEM_COUNT; ++i)
		state->items[i] = ITEM_ABSENT;
	state->items[ITEM_SHEET] = ITEM_IN_ROOM;
}

GameResult GameModel_Command(GameState* state, Language language, const c8* input)
{
	c8 command[31];
	c8* object;
	u8 i = 0;
	u8 j = 0;
	GameItem item;

	while (input[i] == ' ')
		++i;
	while (input[i] && j < sizeof(command) - 1)
	{
		c8 ch = input[i++];
		if (ch >= 'a' && ch <= 'z')
			ch -= 'a' - 'A';
		if (ch != ' ' || (j && command[j - 1] != ' '))
			command[j++] = ch;
	}
	if (input[i])
		return RESULT_UNKNOWN;
	if (j && command[j - 1] == ' ')
		--j;
	command[j] = 0;
	if (!j)
		return RESULT_EMPTY;
	if (matches(command, "N") || matches(command, "S") ||
		equals(command, "L", "E", language) ||
		equals(command, "NORTE", "NORTH", language) ||
		equals(command, "SUL", "SOUTH", language) ||
		equals(command, "LESTE", "EAST", language))
		return RESULT_WALL;
	if (equals(command, "O", "W", language) ||
		equals(command, "OESTE", "WEST", language))
		return RESULT_CORRIDOR;
	if (equals(command, "OLHE", "LOOK", language))
		return RESULT_LOOK;
	if (equals(command, "INVENTARIO", "INVENTORY", language))
		return RESULT_INVENTORY;

	object = command;
	while (*object && *object != ' ')
		++object;
	if (!*object)
		return RESULT_UNKNOWN;
	*object++ = 0;
	if (equals(object, "CAMA", "BED", language))
	{
		if (equals(command, "EXAMINE", "EXAMINE", language))
			return RESULT_EXAMINE_BED;
		if (equals(command, "PEGUE", "TAKE", language))
			return RESULT_FIXED_BED;
		return RESULT_UNKNOWN;
	}
	if (equals(object, "LENCOL", "SHEET", language))
		item = ITEM_SHEET;
	else if (equals(object, "TIRAS", "STRIPES", language))
		item = ITEM_STRIPS;
	else if (equals(object, "CORDA", "ROPE", language))
		item = ITEM_ROPE;
	else
		return RESULT_UNKNOWN;

	if (equals(command, "SOLTE", "LEAVE", language))
	{
		if (state->items[item] != ITEM_CARRIED)
			return RESULT_NOT_CARRIED;
		state->items[item] = ITEM_IN_ROOM;
		return RESULT_DROPPED;
	}
	if (state->items[item] == ITEM_ABSENT)
		return RESULT_NOT_VISIBLE;
	if (equals(command, "PEGUE", "TAKE", language))
	{
		if (state->items[item] == ITEM_CARRIED)
			return RESULT_ALREADY_CARRIED;
		state->items[item] = ITEM_CARRIED;
		return RESULT_TAKEN;
	}
	if (equals(command, "EXAMINE", "EXAMINE", language))
		return RESULT_EXAMINE_ITEM;
	if (item == ITEM_SHEET && (equals(command, "RASGUE", "TEAR", language) ||
		equals(command, "CORTE", "CUT", language)))
	{
		state->items[ITEM_SHEET] = ITEM_ABSENT;
		state->items[ITEM_STRIPS] = ITEM_CARRIED;
		return RESULT_TORN;
	}
	if (item == ITEM_STRIPS && equals(command, "AMARRE", "TIE", language))
	{
		state->items[ITEM_STRIPS] = ITEM_ABSENT;
		state->items[ITEM_ROPE] = ITEM_CARRIED;
		return RESULT_TIED;
	}
	return RESULT_UNKNOWN;
}
