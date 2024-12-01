#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Mapa.h>
#include <Jogo.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

Map *init_map(ALLEGRO_DISPLAY *display) {
  Map *m = malloc(sizeof(Map));

  m->background = al_create_bitmap(MAP_PX_WIDTH, MAP_PX_HEIGHT);
  al_set_target_bitmap(m->background);

  m->tileset = cria_matriz_de_codigos_de_blocos();
  m->objectset = cria_matriz_de_codigos_de_blocos();

  ALLEGRO_BITMAP *background_sprites = al_load_bitmap("assets/background-sprites.png");
  cria_cenario(background_sprites, m);
  al_destroy_bitmap(background_sprites);

  al_set_target_backbuffer(display);

  return m;
}

char ***cria_matriz_de_codigos_de_blocos() {
  char ***mat = calloc(MAP_BLOCK_HEIGHT, sizeof(char **));

  for (int i = 0; i < MAP_BLOCK_HEIGHT; i++) {
    mat[i] = calloc(MAP_BLOCK_WIDTH, sizeof(char *));
    for (int j = 0; j < MAP_BLOCK_WIDTH; j++) {
      mat[i][j] = calloc(2, sizeof(char));
    }
  }

  return mat;
}

void finaliza_matriz_de_codigos_de_blocos(char ***matriz) {
  for (int i = 0; i < MAP_BLOCK_HEIGHT; i++) {
    for (int j = 0; j < MAP_BLOCK_WIDTH; j++) {
      free(matriz[i][j]);
    }
    free(matriz[i]);
  }
  free(matriz);
}

void cria_cenario(ALLEGRO_BITMAP *background_sprites, Map *m) {
  FILE *f = fopen("map.txt", "r");

  if (f == NULL) {
    puts("\033[38;5;196mNao foi possivel abrir o arquivo do mapa. Reinicie o jogo e tente novamente.\033[0m");
  }

  char *s = malloc(sizeof(char) * 2);
  int cont = 0;

  for (int line = 0; line < MAP_BLOCK_HEIGHT; line++) {
    for (int col = 0; col < MAP_BLOCK_WIDTH; col++) {
      fscanf(f, "%s", s);
      strcpy(m->tileset[line][col], s);
      draw_tile(background_sprites, s, col, line);
      if (!bloco_andavel(s) && s[1] != '3') {
        if (line > 0 && !bloco_andavel(m->tileset[line - 1][col])) {
          draw_tile(background_sprites, "A2", col, line);
          draw_tile(background_sprites, "A1", col, line - 1);
        }
        if (line == 0) {
          draw_tile(background_sprites, "A2", col, line);
        }
        if (line == MAP_BLOCK_HEIGHT - 1) {
          draw_tile(background_sprites, "A1", col, line);
        }
      }
      // if (!bloco_andavel(s) && s[1] == '1') {
      //   if (!(line == MAP_BLOCK_HEIGHT - 1 && (0 < col && col < MAP_BLOCK_WIDTH - 1)))
      //     draw_tile(background_sprites, "@1", col, line);
      //   if (!(line == 0 && (0 < col && col < MAP_BLOCK_WIDTH - 1)))
      //     draw_tile(background_sprites, "C1", col, line);
      // }
    }
  }

  free(s);
  fclose(f);
}

void draw_tile(ALLEGRO_BITMAP *background_sprites, const char *block_type, int dx, int dy) {
  int *coord = mapeia_codigo_para_bloco(block_type);
  int sx = coord[0], sy = coord[1];
  al_draw_scaled_bitmap(background_sprites, sx * BLOCK_SPRITE_SIZE, sy * BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, dx * BLOCK_SCALED_SPRITE_SIZE, dy * BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, 0);
  free(coord);
}

int *mapeia_codigo_para_bloco(const char *s) {
  char c;
  int n;
  sscanf(s, "%c%d", &c, &n);
  int x = 0;
  int y = 0;
  printf("c >= 'a' = %d\n", c >= 'a');
  if (c >= 'a' && c <= 'i') {
    x = (int)(c - 'a') % 3 + 5 * (n - 1);
    y = (int)(c - 'a') / 3;
  } else if (c == 'A') {
    x = 0;
    y = 9 + n;
  } else if (c == 'T') {
    x = 3 + (n - 1) % 2;
    y = n / 3;
  }
  // if (x < 0) x *= -1;
  // if (y < 0) y *= -1;
  int *coord = malloc(2 * sizeof(int));
  coord[0] = x;
  coord[1] = y;
  return coord;
}

bool bloco_andavel(const char *block) {
  bool andavel = true;
  //* o tamanho precisa ser 3 pq tem q sobrar espaco pro \0
  // const int size = 17;
  char no_walk_blocks[26][3] = {
    "a1", "b1", "c1",
    "d1", "f1", "g1",
    "h1", "i1", "a3",
    "b3", "c3", "d3",
    "e3", "f3", "g3",
    "h3", "i3", "a2",
    "b2", "c2", "d2",
    "e2", "f2", "g2",
    "h2", "i2"
  };

  for (int i = 0; i < 26; i++) {
    if (strcmp(block, no_walk_blocks[i]) == 0) {
      andavel = false;
      break;
    }

  }

  return andavel;
}

char *get_block_from_position(Map *M, int x, int y) {
  char *c = calloc(2, sizeof(char));
  int col = x / BLOCK_SCALED_SPRITE_SIZE;
  if (col >= MAP_BLOCK_WIDTH) col--;
  int lin = y / BLOCK_SCALED_SPRITE_SIZE;
  if (lin >= MAP_BLOCK_HEIGHT) lin--;
  strcpy(c, M->tileset[lin][col]);
  return c;
}

void finalizar_mapa(Map *m) {
  al_destroy_bitmap(m->background);
  finaliza_matriz_de_codigos_de_blocos(m->tileset);
  finaliza_matriz_de_codigos_de_blocos(m->objectset);
  free(m);
}