#pragma once

#include "msxgl.h"

typedef enum
{
	LANGUAGE_PORTUGUESE,
	LANGUAGE_ENGLISH
} Language;

bool Language_Select(Language* language);
