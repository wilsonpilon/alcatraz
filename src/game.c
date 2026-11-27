#include "game.h"
#include "game_input.h"
#include "game_model.h"
#include "game_ui.h"

void Game_Run(Language language)
{
	GameState state;
	c8 input[31];
	u8 length = 0;
	u8 key;
	GameResult result;

	input[0] = 0;
	GameModel_Init(&state);
	GameUI_Init(&state, language);
	GameInput_Reset();
	for (;;)
	{
		key = GameInput_Read();
		if (key == GAME_INPUT_ESCAPE)
			return;
		if (key == GAME_INPUT_BACKSPACE && length)
		{
			input[--length] = 0;
			GameUI_Input(input);
		}
		else if (key == GAME_INPUT_ENTER)
		{
			result = GameModel_Command(&state, language, input);
			GameUI_Scene(&state);
			GameUI_Inventory(&state, language);
			GameUI_Result(&state, language, result);
			length = 0;
			input[0] = 0;
			GameUI_Input(input);
		}
		else if (key >= GAME_INPUT_NORTH && key <= GAME_INPUT_WEST)
		{
			const c8* direction = key == GAME_INPUT_NORTH ? "N" :
				key == GAME_INPUT_SOUTH ? "S" : key == GAME_INPUT_EAST ?
				(language == LANGUAGE_ENGLISH ? "E" : "L") :
				(language == LANGUAGE_ENGLISH ? "W" : "O");
			result = GameModel_Command(&state, language, direction);
			GameUI_Result(&state, language, result);
		}
		else if (key >= GAME_INPUT_CHARACTER && key <= 'Z')
		{
			if (length < sizeof(input) - 1)
			{
				input[length++] = key;
				input[length] = 0;
				GameUI_Input(input);
			}
			else
				GameUI_Result(&state, language, RESULT_INPUT_FULL);
		}
		Halt();
	}
}
