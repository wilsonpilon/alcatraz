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

void GameUI_Scene(const GameState* state)
{
	u8 x, y;
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

	/* A distant barred corridor is visible through the cell doorway. */
	fill(8, 31, 39, 81, INK);
	fill(12, 35, 35, 77, BLUE);
	for (x = 15; x < 36; x += 7)
		fill(x, 35, x + 1, 77, STEEL);
	line(12, 47, 35, 47, STEEL);
	line(12, 69, 35, 69, STEEL);
	Draw_Box(8, 31, 39, 81, MORTAR, VDP_OP_IMP);

	/* The original describes a masonry bed. */
	fill(55, 71, 133, 87, BED);
	fill(59, 67, 129, 73, BRICK_LIGHT);
	line(55, 87, 133, 87, INK);
	line(61, 88, 61, 94, INK);
	line(128, 88, 128, 94, INK);
	if (state->items[ITEM_SHEET] == ITEM_IN_ROOM)
	{
		fill(62, 64, 117, 70, SHEET);
		line(65, 66, 114, 66, WHITE);
	}
	fill(119, 63, 130, 69, WHITE);
	if (state->items[ITEM_STRIPS] == ITEM_IN_ROOM)
	{
		line(80, 95, 98, 91, WHITE);
		line(83, 98, 104, 94, SHEET);
	}
	if (state->items[ITEM_ROPE] == ITEM_IN_ROOM)
	{
		line(80, 96, 93, 92, GOLD);
		line(93, 92, 102, 97, GOLD);
		line(102, 97, 110, 93, GOLD);
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
		fill(166, y, 177, y + 9, SHEET);
		line(168, y + 2, 175, y + 2, WHITE);
		if (i == ITEM_ROPE)
			line(166, y + 7, 177, y + 3, BED);
		text(182, y + 1,
			i == ITEM_SHEET ? (language == LANGUAGE_ENGLISH ? "SHEET" : "LENCOL") :
			i == ITEM_STRIPS ? (language == LANGUAGE_ENGLISH ? "STRIPES" : "TIRAS") :
			(language == LANGUAGE_ENGLISH ? "ROPE" : "CORDA"), WHITE, PANEL);
		y += 18;
	}
	if (y == 43)
		text(166, 45, language == LANGUAGE_ENGLISH ? "EMPTY" : "VAZIO", WHITE, PANEL);
	VDP_CommandWait();
}

static void description(const GameState* state, Language language)
{
	bool en = language == LANGUAGE_ENGLISH;
	text(8, 119, en ? "YOU ARE IN YOUR CELL." : "VOCE ESTA EM SUA CELA.", WHITE, INK);
	text(8, 130, en ? "HERE: BED" : "AQUI: CAMA", WHITE, INK);
	if (state->items[ITEM_SHEET] == ITEM_IN_ROOM)
		text(68, 130, en ? ", SHEET" : ", LENCOL", WHITE, INK);
	else if (state->items[ITEM_STRIPS] == ITEM_IN_ROOM)
		text(68, 130, en ? ", STRIPES" : ", TIRAS", WHITE, INK);
	else if (state->items[ITEM_ROPE] == ITEM_IN_ROOM)
		text(68, 130, en ? ", ROPE" : ", CORDA", WHITE, INK);
	text(8, 141, en ? "WEST: CORRIDOR WITH A GUARD." : "OESTE: CORREDOR COM GUARDA.", WHITE, INK);
	text(8, 152, en ? "OTHER DIRECTIONS: WALLS." : "DEMAIS DIRECOES: PAREDES.", WHITE, INK);
}

void GameUI_Result(const GameState* state, Language language, GameResult result)
{
	bool en = language == LANGUAGE_ENGLISH;
	const c8* message;
	fill(4, 114, 251, 172, INK);
	if (result == RESULT_LOOK || result == RESULT_EMPTY)
	{
		description(state, language);
		return;
	}
	switch (result)
	{
	case RESULT_INVENTORY: message = en ? "YOUR ITEMS ARE SHOWN ABOVE." : "SEUS ITENS ESTAO ACIMA."; break;
	case RESULT_WALL: message = en ? "A WALL BLOCKS THE WAY." : "UMA PAREDE BLOQUEIA A PASSAGEM."; break;
	case RESULT_CORRIDOR:
		text(8, 119, en ? "WEST: A GUARD IN THE CORRIDOR." : "OESTE: GUARDA NO CORREDOR.", WHITE, INK);
		message = en ? "THIS ROOM IS NOT READY YET." : "ESSA SALA AINDA NAO ESTA PRONTA.";
		text(8, 130, message, WHITE, INK);
		return;
	case RESULT_TAKEN: message = en ? "TAKEN." : "PEGOU."; break;
	case RESULT_DROPPED: message = en ? "LEFT IN THE CELL." : "DEIXOU NA CELA."; break;
	case RESULT_ALREADY_CARRIED: message = en ? "YOU ALREADY HAVE IT." : "VOCE JA ESTA CARREGANDO."; break;
	case RESULT_NOT_CARRIED: message = en ? "YOU ARE NOT CARRYING IT." : "VOCE NAO ESTA CARREGANDO."; break;
	case RESULT_NOT_VISIBLE: message = en ? "YOU CANNOT SEE IT HERE." : "VOCE NAO VE ISSO AQUI."; break;
	case RESULT_FIXED_BED: message = en ? "THE BED CANNOT BE MOVED." : "NAO DA PARA PEGAR A CAMA."; break;
	case RESULT_EXAMINE_BED: message = en ? "THE BED IS MADE OF BRICKS." : "A CAMA E FEITA DE TIJOLOS."; break;
	case RESULT_EXAMINE_ITEM: message = en ? "IT IS ORDINARY." : "E COMUM."; break;
	case RESULT_TORN: message = en ? "THE SHEET IS NOW STRIPES." : "O LENCOL VIROU TIRAS."; break;
	case RESULT_TIED: message = en ? "THE STRIPES BECAME A ROPE." : "AS TIRAS VIRARAM UMA CORDA."; break;
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
	fill(0, 0, 255, 12, RED);
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetMode(PRINT_MODE_BITMAP);
	text(8, 2, language == LANGUAGE_ENGLISH ? "ALCATRAZ  /  CELL" : "ALCATRAZ  /  CELA", WHITE, RED);
	GameUI_Scene(state);
	GameUI_Inventory(state, language);
	fill(4, 109, 251, 183, INK);
	Draw_Box(4, 109, 251, 183, STEEL, VDP_OP_IMP);
	text(8, 176, language == LANGUAGE_ENGLISH ? "LOOK / INVENTORY    ESC: EXIT" : "OLHE / INVENTARIO   ESC: SAIR", GOLD, INK);
	GameUI_Result(state, language, RESULT_LOOK);
	GameUI_Input("");
	VDP_EnableDisplay(TRUE);
}
