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
  char ***tileset;
  char ***objectset;
} Map;

Map *init_map(ALLEGRO_DISPLAY *display);

char ***cria_matriz_de_codigos_de_blocos();

void finaliza_matriz_de_codigos_de_blocos(char ***matriz);

void cria_cenario(ALLEGRO_BITMAP *cenario, Map *m);

void draw_tile(ALLEGRO_BITMAP *background_sprites, const char *block_type, int dx, int dy);

int *mapeia_codigo_para_bloco(const char *s);

bool bloco_andavel(const char *block);

char *get_block_from_position(Map *M, int x, int y);

void finalizar_mapa(Map *m);