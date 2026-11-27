#pragma once

#include "game_model.h"

void GameUI_Init(const GameState* state, Language language);
void GameUI_Inventory(const GameState* state, Language language);
void GameUI_Scene(const GameState* state);
void GameUI_Result(const GameState* state, Language language, GameResult result);
void GameUI_Input(const c8* input);
