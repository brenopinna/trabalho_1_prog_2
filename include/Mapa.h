#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>

#define MAP_BLOCK_WIDTH 16
#define MAP_BLOCK_HEIGHT 12

#define BLOCK_SPRITE_SIZE 128
#define BLOCK_SCALED_SPRITE_SIZE 48

#define MAP_PX_WIDTH (MAP_BLOCK_WIDTH * BLOCK_SCALED_SPRITE_SIZE)
#define MAP_PX_HEIGHT (MAP_BLOCK_HEIGHT * BLOCK_SCALED_SPRITE_SIZE)

typedef struct Map {
  ALLEGRO_BITMAP *background;
} Map;

Map *init_map(ALLEGRO_DISPLAY *display);

int *mapeia_bloco_piso(const char *s);

void draw_tile(ALLEGRO_BITMAP *background_sprites, const char *block_type, int dx, int dy);

void cria_cenario(ALLEGRO_BITMAP *cenario);

void finalizar_mapa(Map *m);