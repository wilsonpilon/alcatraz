#include "game_model.h"
#include "game_map.h"

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
	state->room = ROOM_CELL;
	state->items[ITEM_SHEET] = ROOM_CELL;
	state->guard_gone = FALSE;
	state->rope_cast = FALSE;
	state->gate_open = FALSE;
}

static bool visible(const GameState* state, GameItem item)
{
	return state->items[item] == ITEM_CARRIED || state->items[item] == state->room;
}

static GameResult move(GameState* state, GameDirection direction)
{
	c8 passage = GameMap_Passage(state->room, direction);
	u8 destination = GameMap_Destination(state->room, direction);
	if (!destination)
		return RESULT_WALL;
	if (state->room == ROOM_GUARD && direction == GAME_WEST && !state->gate_open)
		return RESULT_BARS_CLOSED;
	if (!GameMap_RoomReady(destination))
		return RESULT_UNBUILT;
	if (passage != '0' && !(state->room == ROOM_GUARD &&
		direction == GAME_WEST && passage == '2' && state->gate_open))
		return RESULT_UNBUILT;
	state->room = destination;
	return RESULT_MOVED;
}

GameResult GameModel_Command(GameState* state, Language language, const c8* input)
{
	c8 command[31];
	c8* object;
	c8* target;
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
	if (matches(command, "N") || equals(command, "NORTE", "NORTH", language))
		return move(state, GAME_NORTH);
	if (matches(command, "S") || equals(command, "SUL", "SOUTH", language))
		return move(state, GAME_SOUTH);
	if (equals(command, "L", "E", language) || equals(command, "LESTE", "EAST", language))
		return move(state, GAME_EAST);
	if (equals(command, "O", "W", language) ||
		equals(command, "OESTE", "WEST", language))
		return move(state, GAME_WEST);
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
	target = object;
	while (*target && *target != ' ')
		++target;
	if (*target)
		*target++ = 0;
	else
		target = 0;

	if (target)
	{
		if (equals(command, "PECA", "ASK", language) &&
			equals(object, "COMIDA", "FOOD", language) &&
			equals(target, "GUARDA", "OFFICER", language))
		{
			if (state->room != ROOM_GUARD || state->guard_gone)
				return RESULT_NO_GUARD;
			state->guard_gone = TRUE;
			state->items[ITEM_FOOD] = state->room;
			return RESULT_GUARD_LEFT;
		}
		if (equals(command, "AMARRE", "TIE", language) &&
			equals(object, "CORDA", "ROPE", language) &&
			equals(target, "GRADES", "BARS", language))
		{
			if (state->room != ROOM_GUARD)
				return RESULT_NO_BARS;
			if (!visible(state, ITEM_ROPE))
				return RESULT_NOT_VISIBLE;
			return RESULT_ROPE_TIED;
		}
		if (equals(command, "ABRA", "OPEN", language) &&
			equals(object, "GRADES", "BARS", language) &&
			equals(target, "CHAVE", "KEYS", language))
		{
			if (state->room != ROOM_GUARD)
				return RESULT_NO_BARS;
			if (!visible(state, ITEM_KEY))
				return RESULT_NOT_VISIBLE;
			if (state->gate_open)
				return RESULT_GATE_ALREADY;
			state->gate_open = TRUE;
			return RESULT_GATE_OPEN;
		}
		return RESULT_UNKNOWN;
	}
	if (equals(object, "CAMA", "BED", language))
	{
		if (state->room != ROOM_CELL)
			return RESULT_NOT_VISIBLE;
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
	else if (equals(object, "COMIDA", "FOOD", language))
		item = ITEM_FOOD;
	else if (equals(object, "CHAVE", "KEYS", language))
		item = ITEM_KEY;
	else
		return RESULT_UNKNOWN;

	if (equals(command, "SOLTE", "LEAVE", language))
	{
		if (state->items[item] != ITEM_CARRIED)
			return RESULT_NOT_CARRIED;
		state->items[item] = state->room;
		return RESULT_DROPPED;
	}
	if (!visible(state, item))
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
	if (item == ITEM_ROPE && equals(command, "JOGUE", "THROW", language))
	{
		if (state->room != ROOM_GUARD || !state->guard_gone)
			return RESULT_NOTHING;
		state->rope_cast = TRUE;
		return RESULT_ROPE_HIT;
	}
	if (item == ITEM_ROPE && equals(command, "PUXE", "PULL", language))
	{
		if (state->room != ROOM_GUARD || !state->rope_cast ||
			state->items[ITEM_KEY] != ITEM_ABSENT)
			return RESULT_NOTHING;
		state->items[ITEM_KEY] = ITEM_CARRIED;
		return RESULT_KEY_FOUND;
	}
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
