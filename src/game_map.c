#include "game_map.h"
#include "game_model.h"
#include "game_map_data.h"

c8 GameMap_Passage(u8 room, GameDirection direction)
{
	return g_room_passages[room - 1][direction];
}

u8 GameMap_Destination(u8 room, GameDirection direction)
{
	if (GameMap_Passage(room, direction) == '1')
		return 0;
	switch (direction)
	{
	case GAME_NORTH: return room - 14;
	case GAME_SOUTH: return room + 14;
	case GAME_EAST: return room + 1;
	default: return room - 1;
	}
}

bool GameMap_RoomReady(u8 room)
{
	return room == ROOM_CELL || room == ROOM_GUARD || room == ROOM_CORRIDOR;
}
