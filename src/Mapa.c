#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>
#include <Mapa.h>

/*
  Função que carrega um mapa. Os dados são lidos de um arquivo e o cenário
  é renderizado para um bitmap, que fica salvo na memória até o mapa ser
  finalizado. Dessa forma, a cada quadro renderizado, ao invés de ser
  preciso desenhar vários blocos individuais, basta desenhar uma única
  grande imagem na tela para desenhar todo o mapa, e não é preciso reler
  o arquivo constantemente. Retorna um ponteiro para uma struct Map.
*/

Map *iniciar_mapa(ALLEGRO_DISPLAY *display, const char *arquivo_mapa) {
  /* Aloca dinamicamente uma nova struct Map e inicializa-a. */
  Map *m = malloc(sizeof(Map));

  m->background = al_create_bitmap(MAPA_LARGURA_PX, MAPA_ALTURA_PX); // Cria o bitmap onde o mapa é renderizado.
  al_set_target_bitmap(m->background); // Faz com que o Allegro passe a desenhar no bitmap ao invés de desenhar no backbuffer da tela.

  m->tiles = criar_matriz_de_codigos_de_blocos(); // Carrega os dados contidos no arquivo do mapa para uma matriz.

  ALLEGRO_BITMAP *background_sprites = al_load_bitmap("assets/background-sprites.png"); // Carrega no Allegro a imagem dos blocos de cenário.

  /* Caso o jogo não consiga abrir a imagem, uma mensagem
     de erro é exibida e o jogo encerra abruptamente. */
  if (background_sprites == NULL) {
    puts("Nao foi possivel abrir a imagem dos blocos de cenario. Reinicie o jogo e tente novamente.");
  }
  assert(background_sprites != NULL);

  criar_cenario(background_sprites, m, arquivo_mapa); // Lê o arquivo do mapa e renderiza o mapa no bitmap.
  al_destroy_bitmap(background_sprites); // Libera a memória da imagem dos blocos de cenário.

  al_set_target_backbuffer(display); // Faz com que o Allegro volte a desenhar no backbuffer da tela.

  return m;
}

/*
  Função que aloca dinamicamente a memória necessária para a matriz
  de códigos de blocos. Toda a memória alocada é inicializada com
  zeros. Retorna um ponteiro para uma matriz de strings.
*/

char ***criar_matriz_de_codigos_de_blocos() {
  /* Aloca um vetor de ponteiros para as linhas da matriz. */
  char ***mat = calloc(MAPA_ALTURA_BLOCOS, sizeof(char **));

  /* Aloca um vetor de ponteiros para os elementos de uma linha da matriz. */
  for (int i = 0; i < MAPA_ALTURA_BLOCOS; i++) {
    mat[i] = calloc(MAPA_LARGURA_BLOCOS, sizeof(char *));
    for (int j = 0; j < MAPA_LARGURA_BLOCOS; j++) {

      /* Aloca uma string de três caracteres, em que será armazenado o código de um bloco. */
      mat[i][j] = calloc(3, sizeof(char));
    }
  }

  return mat;
}

/*
  Função que libera a memória da matriz de códigos de blocos.
*/

void finalizar_matriz_de_codigos_de_blocos(char ***matriz) {
  for (int i = 0; i < MAPA_ALTURA_BLOCOS; i++) {
    for (int j = 0; j < MAPA_LARGURA_BLOCOS; j++) {
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

void criar_cenario(ALLEGRO_BITMAP *background_sprites, Map *m, const char *arquivo_mapa) {
  /* Abre o arquivo do mapa no modo de leitura. */
  FILE *f = fopen(arquivo_mapa, "r");

  /* Caso o jogo não consiga abrir o arquivo, uma mensagem
     de erro é exibida e o jogo encerra abruptamente. */
  if (f == NULL) {
    puts("Nao foi possivel abrir o arquivo do mapa. Reinicie o jogo e tente novamente.");
  }
  assert(f != NULL);

  char *s = malloc(sizeof(char) * 3); // String temporária que armazena um código de bloco por vez.

  /* Lógica de renderização do mapa para o bitmap. Ela
     é executada em loop, uma vez para cada bloco. */
  for (int line = 0; line < MAPA_ALTURA_BLOCOS; line++) {
    for (int col = 0; col < MAPA_LARGURA_BLOCOS; col++) {
      int n = fscanf(f, "%s", s); // Lê o código de um bloco.
      assert(n != 0); // O jogo encerra abruptamente se a leitura de um bloco falhar.
      strcpy(m->tiles[line][col], s); // Salva o código na matriz de códigos de blocos.
      desenhar_tile(background_sprites, s, col, line); // Renderiza o bloco correspondente para o bitmap.

      /* Lógica de renderização das árvores. Elas são desenhadas por cima do resto do mapa. */
      // Verifica se o bloco que acabou de ser desenhado é um bloco de terra.
      if (s[0] == BLOCO_TERRA) {

        // Se o bloco logo acima dele não for andável, desenha a árvore completa, de cima para baixo.
        if (line > 0 && !bloco_andavel(m->tiles[line - 1][col])) {
          desenhar_tile(background_sprites, "a2", col, line);
          desenhar_tile(background_sprites, "a1", col, line - 1);
        }

        // Se for a primeira linha do cenário, desenha só a parte de baixo da árvore.
        if (line == 0) {
          desenhar_tile(background_sprites, "a2", col, line);
        }

        // Se for a última linha do cenário, desenha só a parte de cima da árvore.
        if (line == MAPA_ALTURA_BLOCOS - 1) {
          desenhar_tile(background_sprites, "a1", col, line);
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

void desenhar_tile(ALLEGRO_BITMAP *background_sprites, const char *tipo_do_bloco, int dx, int dy) {
  int *coord = mapear_codigo_para_bloco(tipo_do_bloco);
  al_draw_scaled_bitmap(background_sprites, coord[0] * BLOCO_TAMANHO_SPRITE, coord[1] * BLOCO_TAMANHO_SPRITE, BLOCO_TAMANHO_SPRITE, BLOCO_TAMANHO_SPRITE,
                        dx * BLOCO_TAMANHO_SPRITE_REDUZIDA, dy * BLOCO_TAMANHO_SPRITE_REDUZIDA, BLOCO_TAMANHO_SPRITE_REDUZIDA, BLOCO_TAMANHO_SPRITE_REDUZIDA, 0);
  free(coord);
}

/*
  Função que recebe o código de um bloco e calcula a posição exata dele
  na imagem dos blocos de cenário. Retorna um ponteiro para um vetor de
  inteiros alocado dinamicamente que contém as coordenadas obtidas.
*/

int *mapear_codigo_para_bloco(const char *s) {
  /* Extrai do código os valores necessários para calcular as coordenadas do bloco. */
  char c; int n, x = 0, y = 0;
  sscanf(s, "%c%d", &c, &n);

  if (c == BLOCO_GRAMA) { // Grama.
    x = (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == BLOCO_TERRA) { // Terra.
    x = 5 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == BLOCO_TERRA_AGUA) { // Água com fundo de terra.
    x = 15 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == BLOCO_GRAMA_QUINA) { // Canto de grama.
    x = 3 + (n - 1) % 2;
    y = (n - 1) / 2;
  } else if (c == BLOCO_AGUA) { // Água.
    x = 10 + (n - 1) % 3;
    y = (n - 1) / 3;
  } else if (c == BLOCO_AGUA_QUINA) { // Canto de água.
    x = 13 + (n - 1) % 2;
    y = (n - 1) / 2;
  } else if (c == BLOCO_ARVORE) { // Árvore.
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

bool bloco_andavel(const char *bloco) {
  /* Lista de blocos não andáveis. */
  bool andavel = bloco[0] != BLOCO_AGUA && // Água.
    bloco[0] != BLOCO_TERRA && // Terra.
    bloco[0] != BLOCO_TERRA_AGUA && // Água com fundo de terra.
    bloco[0] != BLOCO_AGUA_QUINA; // Canto de água.

  return andavel;
}

/*
  Função que verifica que bloco de cenário está em uma determinada posição
  do mapa. Retorna um ponteiro para uma string alocada dinamicamente.
*/

char *pegar_bloco_da_posicao(Map *m, int x, int y) {
  /* Aloca uma string de três caracteres, em que será armazenado o código de um bloco. */
  char *c = calloc(3, sizeof(char));

  /* Com base no tamanho que um bloco de cenário ocupa na tela,
     calcula as coordenadas do bloco em que o pixel está. */
  int col = x / BLOCO_TAMANHO_SPRITE_REDUZIDA;
  int lin = y / BLOCO_TAMANHO_SPRITE_REDUZIDA;

  /* Compensação para caso o cálculo ultrapasse os limites da matriz de códigos de blocos. */
  if (col >= MAPA_LARGURA_BLOCOS) col--;
  if (lin >= MAPA_ALTURA_BLOCOS) lin--;

  /* Copia o código do bloco na coordenada encontrada para a string. */
  strcpy(c, m->tiles[lin][col]);

  return c;
}

/*
  Função que libera a memória da struct Map.
*/

void finalizar_mapa(Map *m) {
  al_destroy_bitmap(m->background); // Libera a memória da imagem dos blocos de cenário.
  finalizar_matriz_de_codigos_de_blocos(m->tiles); // Libera a memória da matriz de códigos de blocos.
  free(m); // Libera a memória da struct Map em si.
}
