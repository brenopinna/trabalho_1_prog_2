#include <stdlib.h>
#include <stdio.h>
#include <Mapa.h>
#include <Jogo.h>
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

int *mapeia_bloco_piso(const char *s) {
  char *c = malloc(sizeof(char));
  int *n = malloc(sizeof(int));
  sscanf(s, "%c%d", c, n);
  int x = (int)(*c - 'a') % 3 + 5 * (*n - 1);
  int y = (int)(*c - 'a') / 3;
  int *coord = malloc(2 * sizeof(int));
  coord[0] = x;
  coord[1] = y;
  free(c);
  free(n);
  return coord;
}

void draw_tile(ALLEGRO_BITMAP *background_sprites, const char *block_type, int dx, int dy) {
  int *coord = mapeia_bloco_piso(block_type);
  int sx = coord[0], sy = coord[1];
  al_draw_scaled_bitmap(background_sprites, sx * BLOCK_SPRITE_SIZE, sy * BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, dx * BLOCK_SCALED_SPRITE_SIZE, dy * BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, 0);
  free(coord);
}

void cria_cenario(ALLEGRO_BITMAP *background_sprites) {
  FILE *f = fopen("map.txt", "r");

  if (f == NULL) {
    puts("\033[38;5;196mNao foi possivel abrir o arquivo do mapa. Reinicie o jogo e tente novamente.\033[0m");
  }

  char *s = malloc(sizeof(char) * 2);

  for (int y = 0; y < MAP_BLOCK_HEIGHT; y++) {
    for (int x = 0; x < MAP_BLOCK_WIDTH; x++) {
      fscanf(f, "%s", s);
      draw_tile(background_sprites, s, x, y);
    }
  }

  free(s);
  fclose(f);
}

void finalizar_mapa(Map *m) {
  al_destroy_bitmap(m->background);
  free(m);
}