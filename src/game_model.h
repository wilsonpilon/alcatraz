#pragma once

#include "msxgl.h"
#include "language.h"

typedef enum
{
	ITEM_SHEET,
	ITEM_STRIPS,
	ITEM_ROPE,
	ITEM_COUNT
} GameItem;

typedef enum
{
	ITEM_ABSENT,
	ITEM_IN_ROOM,
	ITEM_CARRIED
} ItemLocation;

typedef struct
{
	ItemLocation items[ITEM_COUNT];
} GameState;

typedef enum
{
	RESULT_EMPTY,
	RESULT_LOOK,
	RESULT_INVENTORY,
	RESULT_WALL,
	RESULT_CORRIDOR,
	RESULT_TAKEN,
	RESULT_DROPPED,
	RESULT_ALREADY_CARRIED,
	RESULT_NOT_CARRIED,
	RESULT_NOT_VISIBLE,
	RESULT_FIXED_BED,
	RESULT_EXAMINE_BED,
	RESULT_EXAMINE_ITEM,
	RESULT_TORN,
	RESULT_TIED,
	RESULT_INPUT_FULL,
	RESULT_UNKNOWN
} GameResult;

void GameModel_Init(GameState* state);
GameResult GameModel_Command(GameState* state, Language language, const c8* input);
