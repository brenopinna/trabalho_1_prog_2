#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <Mapa.h>
#include <Jogo.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

/*
  Função que carrega um mapa. Os dados são lidos de um arquivo e o cenário
  é renderizado para um bitmap, que fica salvo na memória até o mapa ser
  finalizado. Dessa forma, a cada quadro renderizado, ao invés de ser
  preciso desenhar vários blocos individuais, basta desenhar uma única
  grande imagem na tela para desenhar todo o mapa, e não é preciso reler
  o arquivo constantemente. Retorna um ponteiro para uma struct Map.
*/

Map *init_map(ALLEGRO_DISPLAY *display, const char *map_filename) {
  /* Aloca uma nova struct Map dinamicamente. */
  Map *m = malloc(sizeof(Map));

  m->background = al_create_bitmap(MAP_PX_WIDTH, MAP_PX_HEIGHT); // Cria o bitmap onde o mapa é renderizado.
  al_set_target_bitmap(m->background); // Faz com que o Allegro passe a desenhar no bitmap ao invés de desenhar no backbuffer da tela.

  m->tileset = cria_matriz_de_codigos_de_blocos();

  ALLEGRO_BITMAP *background_sprites = al_load_bitmap("assets/background-sprites.png"); // Carrega a imagem dos blocos de cenário.
  cria_cenario(background_sprites, m, map_filename); // Lê o arquivo do mapa e renderiza o mapa no bitmap.
  al_destroy_bitmap(background_sprites); // Descarrega a imagem dos blocos de cenário.

  al_set_target_backbuffer(display); // Faz com que o Allegro volte a desenhar no backbuffer da tela.

  return m;
}

/*
  Função que aloca a memória necessária para a matriz de códigos
  de blocos. Toda a memória alocada é inicializada com zeros.
  Retorna um ponteiro para uma matriz de strings.
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
  Função que libera a memória da matriz de códigos de blocos.
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
  Função que lê um mapa de um arquivo e desenha o cenário em um bitmap.
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

      // TODO: Renderizar objetos decorativos separadamente.
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
  Função que desenha um único bloco de cenário
  em uma posição específica de um bitmap.
*/

void draw_tile(ALLEGRO_BITMAP *background_sprites, const char *block_type, int dx, int dy) {
  int *coord = mapeia_codigo_para_bloco(block_type);
  int sx = coord[0], sy = coord[1];
  al_draw_scaled_bitmap(background_sprites, sx * BLOCK_SPRITE_SIZE, sy * BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, dx * BLOCK_SCALED_SPRITE_SIZE, dy * BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, 0);
  free(coord);
}

/*
  Função que recebe um código de bloco de cenário e calcula a posição
  exata dele na imagem dos blocos de cenário. Retorna um vetor de
  inteiros alocado dinamicamente contendo as coordenadas obtidas.
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
  Função que recebe um código de bloco de cenário e verifica se o
  jogador pode andar sobre ele ou não. Retorna uma variável booleana.
*/

bool bloco_andavel(const char *block) {
  bool andavel = block[0] != WATER_BLOCK &&
    block[0] != LAND_BLOCK &&
    block[0] != WATER_LAND_BLOCK &&
    block[0] != WATER_BLOCK_CORNER;

  return andavel;
}

/*
  Função que verifica que bloco de cenário está em uma determinada posição
  do mapa. Retorna um ponteiro para uma string alocada dinamicamente.
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
  Função que libera a memória da struct Map.
*/

void finalizar_mapa(Map *m) {
  al_destroy_bitmap(m->background);
  finaliza_matriz_de_codigos_de_blocos(m->tileset);
  free(m);
}