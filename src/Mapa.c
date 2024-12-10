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
  /* Aloca dinamicamente uma nova struct Map e inicializa-a. */
  Map *m = malloc(sizeof(Map));

  m->background = al_create_bitmap(MAP_PX_WIDTH, MAP_PX_HEIGHT); // Cria o bitmap onde o mapa é renderizado.
  al_set_target_bitmap(m->background); // Faz com que o Allegro passe a desenhar no bitmap ao invés de desenhar no backbuffer da tela.

  m->tileset = cria_matriz_de_codigos_de_blocos(); // Carrega os dados contidos no arquivo do mapa para uma matriz.

  ALLEGRO_BITMAP *background_sprites = al_load_bitmap("assets/background-sprites.png"); // Carrega no Allegro a imagem dos blocos de cenário.
  cria_cenario(background_sprites, m, map_filename); // Lê o arquivo do mapa e renderiza o mapa no bitmap.
  al_destroy_bitmap(background_sprites); // Libera a memória da imagem dos blocos de cenário.

  al_set_target_backbuffer(display); // Faz com que o Allegro volte a desenhar no backbuffer da tela.

  return m;
}

/*
  Função que aloca dinamicamente a memória necessária para a matriz
  de códigos de blocos. Toda a memória alocada é inicializada com
  zeros. Retorna um ponteiro para uma matriz de strings.
*/

char ***cria_matriz_de_codigos_de_blocos() {
  /* Aloca um vetor de ponteiros para as linhas da matriz. */
  char ***mat = calloc(MAP_BLOCK_HEIGHT, sizeof(char **)); 
  
  /* Aloca um vetor de ponteiros para os elementos de uma linha da matriz. */
  for (int i = 0; i < MAP_BLOCK_HEIGHT; i++) {
    mat[i] = calloc(MAP_BLOCK_WIDTH, sizeof(char *)); // 
    for (int j = 0; j < MAP_BLOCK_WIDTH; j++) {

      /* Aloca uma string de três caracteres, em que será armazenado o código de um bloco. */
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
      /* Libera a memória de uma string da matriz (um bloco). */
      free(matriz[i][j]);
    }
    /* Libera a memória do vetor de ponteiros de uma linha da matriz. */
    free(matriz[i]);
  }
  /* Libera a memória do vetor de ponteiros para as linhas da matriz. */
  free(matriz); 
}

/*
  Função que lê um mapa de um arquivo e desenha o cenário em um bitmap,
  bloco por bloco, de cima para baixo e da esquerda para a direita.
*/

void cria_cenario(ALLEGRO_BITMAP *background_sprites, Map *m, const char *map_filename) {
  /* Abre o arquivo do mapa no modo de leitura. */
  FILE *f = fopen(map_filename, "r");

  /* Mensagem de erro exibida caso o jogo não consiga abrir o arquivo do mapa ou não o encontre. */
  if (f == NULL) {
    puts("Nao foi possivel abrir o arquivo do mapa. Reinicie o jogo e tente novamente.");
  }

  char *s = malloc(sizeof(char) * 3); // String temporária que armazena um código de bloco por vez.

  /* Lógica de renderização do mapa para o bitmap. Ela
     é executada em loop, uma vez para cada bloco. */
  for (int line = 0; line < MAP_BLOCK_HEIGHT; line++) {
    for (int col = 0; col < MAP_BLOCK_WIDTH; col++) {
      int n = fscanf(f, "%s", s); // Lê o código de um bloco.
      assert(n != 0); // O jogo é encerrado abruptamente a leitura de um bloco falhar.
      strcpy(m->tileset[line][col], s); // Salva o código na matriz de códigos de blocos.
      draw_tile(background_sprites, s, col, line); // Renderiza o bloco correspondente para o bitmap.

      // TODO: Renderizar objetos decorativos separadamente.

      /* Lógica de renderização das árvores. Elas são desenhadas por cima do resto do mapa. */
      // Verifica se o bloco que acabou de ser desenhado é um bloco de terra.
      if (s[0] == LAND_BLOCK) {

        // Se o bloco logo acima dele não for andável, desenha a árvore completa, de cima para baixo.
        if (line > 0 && !bloco_andavel(m->tileset[line - 1][col])) {
          draw_tile(background_sprites, "a2", col, line);
          draw_tile(background_sprites, "a1", col, line - 1);
        }

        // Se for a primeira linha do cenário, desenha só a parte de baixo da árvore.
        if (line == 0) {
          draw_tile(background_sprites, "a2", col, line);
        }

        // Se for a última linha do cenário, desenha só a parte de cima da árvore.
        if (line == MAP_BLOCK_HEIGHT - 1) {
          draw_tile(background_sprites, "a1", col, line);
        }
      }
    }
  }

  free(s);

  /* Fecha o arquivo do mapa. */
  fclose(f);
}

/*
  Função que desenha um único bloco de cenário
  em uma posição específica de um bitmap.
*/

void draw_tile(ALLEGRO_BITMAP *background_sprites, const char *block_type, int dx, int dy) {
  int *coord = mapeia_codigo_para_bloco(block_type);
  al_draw_scaled_bitmap(background_sprites, coord[0] * BLOCK_SPRITE_SIZE, coord[1] * BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE,
      dx * BLOCK_SCALED_SPRITE_SIZE, dy * BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, BLOCK_SCALED_SPRITE_SIZE, 0);
  free(coord);
}

/*
  Função que recebe o código de um bloco e calcula a posição exata dele
  na imagem dos blocos de cenário. Retorna um ponteiro para um vetor de
  inteiros alocado dinamicamente que contém as coordenadas obtidas.
*/

int *mapeia_codigo_para_bloco(const char *s) {
  /* Extrai do código os valores necessários para calcular as coordenadas do bloco. */
  char c; int n, x = 0, y = 0;
  sscanf(s, "%c%d", &c, &n);

  if (c == GRASS_BLOCK) { // Grama.
    x = (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == LAND_BLOCK) { // Terra.
    x = 5 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == WATER_LAND_BLOCK) { // Água com fundo de terra.
    x = 15 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == GRASS_BLOCK_CORNER) { // Canto de grama.
    x = 3 + (n - 1) % 2;
    y = (n - 1) / 2;
  } else if (c == WATER_BLOCK) { // Água.
    x = 10 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == WATER_BLOCK_CORNER) { // Canto de água.
    x = 13 + (n - 1) % 2;
    y = (n - 1) / 2;
  } else if (c == TREE_BLOCK) { // Árvore.
    x = 0;
    y = 9 + n;
  }

  int *coord = malloc(2 * sizeof(int));
  coord[0] = x;
  coord[1] = y;
  return coord;
}

/*
  Função que recebe o código de um bloco e verifica se o jogador
  pode andar sobre ele ou não. Retorna um valor booleano.
*/

bool bloco_andavel(const char *block) {
  /* Lista de blocos não andáveis. */
  bool andavel = block[0] != WATER_BLOCK && // Água.
    block[0] != LAND_BLOCK && // Terra.
    block[0] != WATER_LAND_BLOCK && // Água com fundo de terra.
    block[0] != WATER_BLOCK_CORNER; // Canto de água.

  return andavel;
}

/*
  Função que verifica que bloco de cenário está em uma determinada posição
  do mapa. Retorna um ponteiro para uma string alocada dinamicamente.
*/

char *get_block_from_position(Map *M, int x, int y) {
  /* Aloca uma string de três caracteres, em que será armazenado o código de um bloco. */
  char *c = calloc(3, sizeof(char));

  /* Com base no tamanho que um bloco de cenário ocupa na tela,
     calcula as coordenadas do bloco em que o pixel está. */
  int col = x / BLOCK_SCALED_SPRITE_SIZE;
  int lin = y / BLOCK_SCALED_SPRITE_SIZE;

  /* Compensação para caso o cálculo ultrapasse os limites da matriz de códigos de blocos. */
  if (col >= MAP_BLOCK_WIDTH) col--;
  if (lin >= MAP_BLOCK_HEIGHT) lin--;
  
  /* Copia o código do bloco na coordenada encontrada para a string. */
  strcpy(c, M->tileset[lin][col]);

  return c;
}

/*
  Função que libera a memória da struct Map.
*/

void finalizar_mapa(Map *m) {
  al_destroy_bitmap(m->background); // Libera a memória da imagem dos blocos de cenário.
  finaliza_matriz_de_codigos_de_blocos(m->tileset); // Libera a memória da matriz de códigos de blocos.
  free(m); // Libera a memória da struct Map em si.
}
