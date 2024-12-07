#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <Mapa.h>
#include <Jogo.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

/*
  Função que carrega um mapa. Retorna uma struct Map.
*/

Map *init_map(ALLEGRO_DISPLAY *display, const char *map_filename) {
  /* Aloca uma nova struct Map dinamicamente. */
  Map *m = malloc(sizeof(Map));

  m->background = al_create_bitmap(MAP_PX_WIDTH, MAP_PX_HEIGHT);
  al_set_target_bitmap(m->background);

  m->tileset = cria_matriz_de_codigos_de_blocos();
  m->objectset = cria_matriz_de_codigos_de_blocos();

  ALLEGRO_BITMAP *background_sprites = al_load_bitmap("assets/background-sprites.png");
  cria_cenario(background_sprites, m, map_filename);
  al_destroy_bitmap(background_sprites);

  al_set_target_backbuffer(display);

  return m;
}

/*
  Função que
*/

char ***cria_matriz_de_codigos_de_blocos() {
  char ***mat = calloc(MAP_BLOCK_HEIGHT, sizeof(char **));

  for (int i = 0; i < MAP_BLOCK_HEIGHT; i++) {
    mat[i] = calloc(MAP_BLOCK_WIDTH, sizeof(char *));
    for (int j = 0; j < MAP_BLOCK_WIDTH; j++) {
      mat[i][j] = calloc(3, sizeof(char));
    }
  }

  return mat;
}

/*
  Função que
*/

void finaliza_matriz_de_codigos_de_blocos(char ***matriz) {
  for (int i = 0; i < MAP_BLOCK_HEIGHT; i++) {
    for (int j = 0; j < MAP_BLOCK_WIDTH; j++) {
      free(matriz[i][j]);
    }
    free(matriz[i]);
  }
  free(matriz);
}

/*
  Função que
*/

void cria_cenario(ALLEGRO_BITMAP *background_sprites, Map *m, const char *map_filename) {
  FILE *f = fopen(map_filename, "r");

  if (f == NULL) {
    puts("Nao foi possivel abrir o arquivo do mapa. Reinicie o jogo e tente novamente.");
  }

  char *s = malloc(sizeof(char) * 3);

  for (int line = 0; line < MAP_BLOCK_HEIGHT; line++) {
    for (int col = 0; col < MAP_BLOCK_WIDTH; col++) {
      int n = fscanf(f, "%s", s);
      assert(n != 0);
      strcpy(m->tileset[line][col], s);
      draw_tile(background_sprites, s, col, line);
      if (!bloco_andavel(s) && s[0] != WATER_BLOCK && s[0] != WATER_BLOCK_CORNER && s[0] != WATER_LAND_BLOCK) {
        if (line > 0 && !bloco_andavel(m->tileset[line - 1][col])) {
          draw_tile(background_sprites, "a2", col, line);
          draw_tile(background_sprites, "a1", col, line - 1);
        }
        if (line == 0) {
          draw_tile(background_sprites, "a2", col, line);
        }
        if (line == MAP_BLOCK_HEIGHT - 1) {
          draw_tile(background_sprites, "a1", col, line);
        }
      }
    }
  }

  free(s);
  fclose(f);
}

/*
  Função que
*/

void draw_tile(ALLEGRO_BITMAP *background_sprites, const char *block_type, int dx, int dy) {
  int *coord = mapeia_codigo_para_bloco(block_type);
  int sx = coord[0], sy = coord[1];
  al_draw_scaled_bitmap(background_sprites, sx * BLOCK_SPRITE_SIZE, sy * BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, dx * BLOCK_SCALED_SPRITE_SIZE, dy * BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, 0);
  free(coord);
}

/*
  Função que
*/

int *mapeia_codigo_para_bloco(const char *s) {
  char c; int n, x = 0, y = 0;
  sscanf(s, "%c%d", &c, &n);

  if (c == GRASS_BLOCK) {
    x = (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == LAND_BLOCK) {
    x = 5 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == WATER_LAND_BLOCK) {
    x = 15 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == GRASS_BLOCK_CORNER) {
    x = 3 + (n - 1) % 2;
    y = (n - 1) / 2;
  } else if (c == WATER_BLOCK) {
    x = 10 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == WATER_BLOCK_CORNER) {
    x = 13 + (n - 1) % 2;
    y = (n - 1) / 2;
  } else if (c == TREE_BLOCK) {
    x = 0;
    y = 9 + n;
  }

  int *coord = malloc(2 * sizeof(int));
  coord[0] = x;
  coord[1] = y;
  return coord;
}

/*
  Função que
*/

bool bloco_andavel(const char *block) {
  bool andavel = block[0] != WATER_BLOCK &&
    block[0] != LAND_BLOCK &&
    block[0] != WATER_LAND_BLOCK &&
    block[0] != WATER_BLOCK_CORNER;

  return andavel;
}

/*
  Função que
*/

char *get_block_from_position(Map *M, int x, int y) {
  char *c = calloc(3, sizeof(char));
  int col = x / BLOCK_SCALED_SPRITE_SIZE;
  if (col >= MAP_BLOCK_WIDTH) col--;
  int lin = y / BLOCK_SCALED_SPRITE_SIZE;
  if (lin >= MAP_BLOCK_HEIGHT) lin--;
  strcpy(c, M->tileset[lin][col]);
  return c;
}

/*
  Função que
*/

void finalizar_mapa(Map *m) {
  al_destroy_bitmap(m->background);
  finaliza_matriz_de_codigos_de_blocos(m->tileset);
  finaliza_matriz_de_codigos_de_blocos(m->objectset);
  free(m);
}