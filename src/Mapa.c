#include <stdlib.h>
#include <Mapa.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

Map *init_map(ALLEGRO_DISPLAY *display) {
  Map *m = malloc(sizeof(Map));

  m->background = al_create_bitmap(MAP_PX_WIDTH, MAP_PX_HEIGHT);
  al_set_target_bitmap(m->background);

  ALLEGRO_BITMAP *background_sprites = al_load_bitmap("assets/background-sprites.png");
  cria_cenario(background_sprites);
  al_destroy_bitmap(background_sprites);

  al_set_target_backbuffer(display);

  return m;
}

// TODO: Uma funcao para mapear um caractere para coordenadas do bloco, mudando
// TODO: sx e sy para um char que no interior da funcao vira um sx e sy.

void draw_tile(ALLEGRO_BITMAP *background_sprites, int sx, int sy, int dx, int dy) {
  al_draw_scaled_bitmap(background_sprites, sx * BLOCK_SPRITE_SIZE, sy * BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, dx * BLOCK_SCALED_SPRITE_SIZE, dy * BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, 0);
}

void cria_cenario(ALLEGRO_BITMAP *background_sprites) {
  draw_tile(background_sprites, 0, 0, 0, 0);
  for (int i = 1; i < MAP_BLOCK_HEIGHT - 1; i++) {
    draw_tile(background_sprites, 0, 1, 0, i);
  }
  draw_tile(background_sprites, 0, 2, 0, MAP_BLOCK_HEIGHT - 1);

  for (int i = 1; i < MAP_BLOCK_WIDTH - 1; i++) {
    draw_tile(background_sprites, 1, 0, i, 0);
    for (int j = 1; j < MAP_BLOCK_HEIGHT - 1; j++) {
      draw_tile(background_sprites, 1, 1, i, j);
    }
    draw_tile(background_sprites, 1, 2, i, MAP_BLOCK_HEIGHT - 1);
  }

  draw_tile(background_sprites, 2, 0, MAP_BLOCK_WIDTH - 1, 0);
  for (int i = 1; i < MAP_BLOCK_HEIGHT - 1; i++) {
    draw_tile(background_sprites, 2, 1, MAP_BLOCK_WIDTH - 1, i);
  }
  draw_tile(background_sprites, 2, 2, MAP_BLOCK_WIDTH - 1, MAP_BLOCK_HEIGHT - 1);
}

void finalizar_mapa(Map *m) {
  al_destroy_bitmap(m->background);
  free(m);
}