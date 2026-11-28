#pragma once

#include "msxgl.h"

typedef enum
{
	GAME_NORTH,
	GAME_SOUTH,
	GAME_EAST,
	GAME_WEST
} GameDirection;

enum { GAME_ROOM_COUNT = 140 };

c8 GameMap_Passage(u8 room, GameDirection direction);
u8 GameMap_Destination(u8 room, GameDirection direction);
bool GameMap_RoomReady(u8 room);
