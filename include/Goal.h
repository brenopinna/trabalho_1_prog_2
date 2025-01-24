#pragma once

#include <Entity.h>

#define GOAL_TAMANHO_SPRITE          416
#define GOAL_TAMANHO_SPRITE_REDUZIDA  64

#define GOAL_POSICAO_X              GOAL_TAMANHO_SPRITE_REDUZIDA * 9
#define GOAL_POSICAO_Y              GOAL_TAMANHO_SPRITE_REDUZIDA * 7

Entity *criar_objetivo();