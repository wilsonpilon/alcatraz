#include "game_ui.h"
#include "draw.h"

extern const u8 g_Font_MGL_Sample6[];

enum { BLACK, INK, CEILING, MORTAR, BRICK, BRICK_LIGHT, FLOOR, FLOOR_LIGHT,
	STEEL, WHITE, GOLD, BED, SHEET, BLUE, RED, PANEL };

static const u8 palette[32] = {
	0x00,0, 0x11,1, 0x22,2, 0x32,2,
	0x42,2, 0x53,3, 0x04,1, 0x16,3,
	0x44,4, 0x77,7, 0x63,6, 0x43,3,
	0x66,6, 0x26,4, 0x60,1, 0x12,1
};

static void fill(u16 x, u16 y, u16 right, u16 bottom, u8 color)
{
	Draw_FillBox(x, y, right, bottom, color, VDP_OP_IMP);
}

static void line(u16 x, u16 y, u16 right, u16 bottom, u8 color)
{
	Draw_Line(x, y, right, bottom, color, VDP_OP_IMP);
}

static void text(u16 x, u16 y, const c8* value, u8 color, u8 background)
{
	Print_SetColor(color, background);
	Print_SetPosition(x, y);
	Print_DrawText(value);
}

static const c8* item_name(u8 item, Language language)
{
	bool en = language == LANGUAGE_ENGLISH;
	switch (item)
	{
	case ITEM_SHEET: return en ? "SHEET" : "LENCOL";
	case ITEM_STRIPS: return en ? "STRIPES" : "TIRAS";
	case ITEM_ROPE: return en ? "ROPE" : "CORDA";
	case ITEM_FOOD: return en ? "FOOD" : "COMIDA";
	default: return en ? "KEYS" : "CHAVE";
	}
}

void GameUI_Scene(const GameState* state, Language language)
{
	u8 x, y;
	fill(0, 0, 255, 12, RED);
	text(8, 2, state->room == ROOM_CELL ?
		(language == LANGUAGE_ENGLISH ? "ALCATRAZ  /  CELL" : "ALCATRAZ  /  CELA") :
		state->room == ROOM_GUARD ?
		(language == LANGUAGE_ENGLISH ? "ALCATRAZ  /  BARS" : "ALCATRAZ  /  GRADES") :
		(language == LANGUAGE_ENGLISH ? "ALCATRAZ  /  CORRIDOR" : "ALCATRAZ  /  CORREDOR"), WHITE, RED);
	fill(4, 16, 151, 103, BRICK);
	fill(4, 16, 151, 25, CEILING);
	line(4, 25, 151, 25, STEEL);
	for (y = 34; y <= 76; y += 9)
	{
		line(4, y, 151, y, MORTAR);
		for (x = (y & 1) ? 13 : 4; x < 150; x += 24)
			line(x, y - 8, x, y, MORTAR);
	}
	fill(4, 82, 151, 103, FLOOR);
	line(4, 82, 151, 82, FLOOR_LIGHT);
	line(4, 103, 55, 82, FLOOR_LIGHT);
	line(100, 82, 151, 103, FLOOR_LIGHT);
	line(4, 96, 151, 96, FLOOR_LIGHT);

	if (state->room == ROOM_CELL)
	{
		fill(8, 31, 39, 81, INK);
		fill(12, 35, 35, 77, BLUE);
		for (x = 15; x < 36; x += 7)
			fill(x, 35, x + 1, 77, STEEL);
		line(12, 47, 35, 47, STEEL);
		line(12, 69, 35, 69, STEEL);
		Draw_Box(8, 31, 39, 81, MORTAR, VDP_OP_IMP);
		fill(55, 71, 133, 87, BED);
		fill(59, 67, 129, 73, BRICK_LIGHT);
		line(55, 87, 133, 87, INK);
		line(61, 88, 61, 94, INK);
		line(128, 88, 128, 94, INK);
		if (state->items[ITEM_SHEET] == state->room)
		{
			fill(62, 64, 117, 70, SHEET);
			line(65, 66, 114, 66, WHITE);
		}
		fill(119, 63, 130, 69, WHITE);
	}
	else
	{
		fill(48, 34, 98, 81, INK);
		fill(54, 39, 92, 80, BLUE);
		line(4, 26, 48, 34, STEEL);
		line(151, 26, 98, 34, STEEL);
		line(4, 82, 48, 81, FLOOR_LIGHT);
		line(151, 82, 98, 81, FLOOR_LIGHT);
		x = state->room == ROOM_GUARD ? 12 : 113;
		fill(x, 31, x + 29, 81, INK);
		Draw_Box(x, 31, x + 29, 81, STEEL, VDP_OP_IMP);
		if (!state->gate_open)
		{
			for (y = x + 4; y < x + 28; y += 6)
				fill(y, 33, y + 1, 79, STEEL);
			line(x + 1, 48, x + 28, 48, STEEL);
			line(x + 1, 69, x + 28, 69, STEEL);
		}
		else
		{
			line(x + 4, 33, x + 13, 79, STEEL);
			line(x + 10, 33, x + 19, 79, STEEL);
		}
		if (state->room == ROOM_GUARD && !state->guard_gone)
		{
			fill(108, 38, 118, 47, GOLD);
			fill(105, 34, 121, 37, INK);
			fill(108, 30, 118, 33, BLUE);
			fill(104, 48, 122, 67, BLUE);
			fill(109, 49, 116, 62, FLOOR_LIGHT);
			fill(105, 68, 110, 82, INK);
			fill(116, 68, 121, 82, INK);
			fill(101, 51, 104, 65, GOLD);
			fill(122, 51, 125, 65, GOLD);
		}
	}
	if (state->items[ITEM_SHEET] == state->room && state->room != ROOM_CELL)
		fill(65, 91, 83, 96, SHEET);
	if (state->items[ITEM_STRIPS] == state->room)
	{
		line(80, 95, 98, 91, WHITE);
		line(83, 98, 104, 94, SHEET);
	}
	if (state->items[ITEM_ROPE] == state->room)
	{
		line(80, 96, 93, 92, GOLD);
		line(93, 92, 102, 97, GOLD);
		line(102, 97, 110, 93, GOLD);
	}
	if (state->items[ITEM_FOOD] == state->room)
	{
		fill(48, 88, 61, 94, BED);
		line(50, 89, 59, 89, GOLD);
	}
	if (state->items[ITEM_KEY] == state->room)
	{
		line(114, 95, 125, 95, GOLD);
		Draw_Box(111, 92, 115, 97, GOLD, VDP_OP_IMP);
		line(123, 95, 123, 98, GOLD);
	}
	VDP_CommandWait();
}

void GameUI_Inventory(const GameState* state, Language language)
{
	u8 i;
	u8 y = 43;
	fill(158, 16, 251, 103, PANEL);
	Draw_Box(158, 16, 251, 103, STEEL, VDP_OP_IMP);
	text(164, 21, language == LANGUAGE_ENGLISH ? "INVENTORY" : "INVENTARIO", GOLD, PANEL);
	for (i = 0; i < ITEM_COUNT; ++i)
	{
		if (state->items[i] != ITEM_CARRIED)
			continue;
		if (i == ITEM_FOOD)
		{
			fill(166, y + 2, 177, y + 8, BED);
			line(168, y + 3, 175, y + 3, GOLD);
		}
		else if (i == ITEM_KEY)
		{
			Draw_Box(166, y + 1, 170, y + 7, GOLD, VDP_OP_IMP);
			line(171, y + 4, 177, y + 4, GOLD);
			line(176, y + 4, 176, y + 7, GOLD);
		}
		else if (i == ITEM_ROPE)
		{
			line(166, y + 7, 172, y + 2, GOLD);
			line(172, y + 2, 177, y + 7, GOLD);
		}
		else
		{
			fill(166, y, 177, y + 9, SHEET);
			line(168, y + 2, 175, y + 2, WHITE);
		}
		text(182, y + 1, item_name(i, language), WHITE, PANEL);
		y += 18;
	}
	if (y == 43)
		text(166, 45, language == LANGUAGE_ENGLISH ? "EMPTY" : "VAZIO", WHITE, PANEL);
	VDP_CommandWait();
}

static void description(const GameState* state, Language language)
{
	c8 objects[41];
	u8 len = 0;
	u8 i;
	bool any = FALSE;
	bool en = language == LANGUAGE_ENGLISH;
	const c8* word;
	text(8, 119, state->room == ROOM_CELL ?
		(en ? "YOU ARE IN YOUR CELL." : "VOCE ESTA EM SUA CELA.") :
		state->room == ROOM_GUARD ?
		(en ? "YOU ARE BY THE BARS." : "VOCE ESTA JUNTO AS GRADES.") :
		(en ? "YOU ARE IN THE CORRIDOR." : "VOCE ESTA NO CORREDOR."), WHITE, INK);
	word = en ? "HERE: " : "AQUI: ";
	while (*word) objects[len++] = *word++;
	for (i = 0; i < ITEM_COUNT + 2; ++i)
	{
		if (i == 0)
		{
			if (state->room != ROOM_CELL) continue;
			word = en ? "BED" : "CAMA";
		}
		else if (i == 1)
		{
			if (state->room != ROOM_GUARD || state->guard_gone) continue;
			word = en ? "OFFICER" : "GUARDA";
		}
		else
		{
			if (state->items[i - 2] != state->room) continue;
			word = item_name(i - 2, language);
		}
		if (any && len < sizeof(objects) - 2) { objects[len++] = ','; objects[len++] = ' '; }
		while (*word && len < sizeof(objects) - 1) objects[len++] = *word++;
		any = TRUE;
	}
	if (!any)
	{
		word = en ? "NOTHING" : "NADA";
		while (*word) objects[len++] = *word++;
	}
	objects[len] = 0;
	text(8, 130, objects, WHITE, INK);
	if (state->room == ROOM_CELL)
	{
		text(8, 141, en ? "WEST: BARS AND CORRIDOR." : "OESTE: GRADES E CORREDOR.", WHITE, INK);
		text(8, 152, en ? "OTHER DIRECTIONS: WALLS." : "DEMAIS DIRECOES: PAREDES.", WHITE, INK);
	}
	else if (state->room == ROOM_GUARD)
	{
		text(8, 141, en ? "EAST: CELL. WEST: CORRIDOR." : "LESTE: CELA. OESTE: CORREDOR.", WHITE, INK);
		text(8, 152, state->gate_open ?
			(en ? "THE BARS ARE OPEN." : "AS GRADES ESTAO ABERTAS.") :
			(en ? "THE BARS ARE CLOSED." : "AS GRADES ESTAO FECHADAS."), WHITE, INK);
	}
	else
	{
		text(8, 141, en ? "EAST: BARS. NORTH/SOUTH: HALL." : "LESTE: GRADES. N/S: CORREDOR.", WHITE, INK);
		text(8, 152, en ? "NORTH/SOUTH: NOT READY YET." : "NORTE/SUL: AINDA NAO PRONTOS.", WHITE, INK);
	}
}

void GameUI_Result(const GameState* state, Language language, GameResult result)
{
	bool en = language == LANGUAGE_ENGLISH;
	const c8* message;
	fill(4, 114, 251, 172, INK);
	if (result == RESULT_LOOK || result == RESULT_EMPTY || result == RESULT_MOVED)
	{
		description(state, language);
		return;
	}
	switch (result)
	{
	case RESULT_INVENTORY: message = en ? "YOUR ITEMS ARE SHOWN ABOVE." : "SEUS ITENS ESTAO ACIMA."; break;
	case RESULT_WALL: message = en ? "A WALL BLOCKS THE WAY." : "UMA PAREDE BLOQUEIA A PASSAGEM."; break;
	case RESULT_UNBUILT: message = en ? "THIS ROOM IS NOT READY YET." : "ESSA SALA AINDA NAO ESTA PRONTA."; break;
	case RESULT_BARS_CLOSED: message = en ? "THE BARS BLOCK YOUR WAY." : "AS GRADES IMPEDEM A PASSAGEM."; break;
	case RESULT_TAKEN: message = en ? "TAKEN." : "PEGOU."; break;
	case RESULT_DROPPED: message = en ? "LEFT HERE." : "DEIXOU AQUI."; break;
	case RESULT_ALREADY_CARRIED: message = en ? "YOU ALREADY HAVE IT." : "VOCE JA ESTA CARREGANDO."; break;
	case RESULT_NOT_CARRIED: message = en ? "YOU ARE NOT CARRYING IT." : "VOCE NAO ESTA CARREGANDO."; break;
	case RESULT_NOT_VISIBLE: message = en ? "YOU CANNOT SEE IT HERE." : "VOCE NAO VE ISSO AQUI."; break;
	case RESULT_FIXED_BED: message = en ? "THE BED CANNOT BE MOVED." : "NAO DA PARA PEGAR A CAMA."; break;
	case RESULT_EXAMINE_BED: message = en ? "THE BED IS MADE OF BRICKS." : "A CAMA E FEITA DE TIJOLOS."; break;
	case RESULT_EXAMINE_ITEM: message = en ? "IT IS ORDINARY." : "E COMUM."; break;
	case RESULT_TORN: message = en ? "THE SHEET IS NOW STRIPES." : "O LENCOL VIROU TIRAS."; break;
	case RESULT_TIED: message = en ? "THE STRIPES BECAME A ROPE." : "AS TIRAS VIRARAM UMA CORDA."; break;
	case RESULT_GUARD_LEFT:
		text(8, 119, en ? "THE OFFICER BROUGHT FOOD" : "O GUARDA TROUXE COMIDA", WHITE, INK);
		text(8, 130, en ? "AND LEFT THE KEYS OUTSIDE." : "E DEIXOU A CHAVE LA FORA.", WHITE, INK);
		return;
	case RESULT_NO_GUARD: message = en ? "THE OFFICER IS NOT HERE." : "O GUARDA NAO ESTA AQUI."; break;
	case RESULT_NO_BARS: message = en ? "THERE ARE NO BARS HERE." : "NAO HA GRADES AQUI."; break;
	case RESULT_ROPE_TIED: message = en ? "THE ROPE IS TIED TO THE BARS." : "CORDA AMARRADA NAS GRADES."; break;
	case RESULT_ROPE_HIT: message = en ? "THE ROPE TOUCHED SOMETHING." : "A CORDA TOCOU EM ALGO."; break;
	case RESULT_NOTHING: message = en ? "NOTHING HAPPENED." : "NADA ACONTECEU."; break;
	case RESULT_KEY_FOUND: message = en ? "THE KEYS CAME WITH THE ROPE!" : "A CHAVE VEIO COM A CORDA!"; break;
	case RESULT_GATE_OPEN: message = en ? "THE BARS ARE OPEN!" : "AS GRADES ABRIRAM!"; break;
	case RESULT_GATE_ALREADY: message = en ? "THE BARS ARE ALREADY OPEN." : "AS GRADES JA ESTAO ABERTAS."; break;
	case RESULT_INPUT_FULL: message = en ? "LIMIT: 30 LETTERS OR SPACES." : "LIMITE: 30 LETRAS OU ESPACOS."; break;
	default: message = en ? "COMMAND NOT AVAILABLE YET." : "COMANDO AINDA NAO DISPONIVEL."; break;
	}
	text(8, 119, message, WHITE, INK);
}

void GameUI_Input(const c8* input)
{
	fill(4, 188, 251, 205, BLACK);
	text(8, 193, ">", GOLD, BLACK);
	text(20, 193, input, WHITE, BLACK);
	VDP_CommandWait();
}

void GameUI_Init(const GameState* state, Language language)
{
	VDP_EnableDisplay(FALSE);
	VDP_SetPalette(palette);
	VDP_CommandHMMV(0, 0, 256, 212, 0);
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetMode(PRINT_MODE_BITMAP);
	GameUI_Scene(state, language);
	GameUI_Inventory(state, language);
	fill(4, 109, 251, 183, INK);
	Draw_Box(4, 109, 251, 183, STEEL, VDP_OP_IMP);
	text(8, 176, language == LANGUAGE_ENGLISH ? "LOOK / INVENTORY    ESC: EXIT" : "OLHE / INVENTARIO   ESC: SAIR", GOLD, INK);
	GameUI_Result(state, language, RESULT_LOOK);
	GameUI_Input("");
	VDP_EnableDisplay(TRUE);
}
