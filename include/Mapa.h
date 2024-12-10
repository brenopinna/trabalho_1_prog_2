#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>

#define MAPA_LARGURA_BLOCOS 16
#define MAPA_ALTURA_BLOCOS 12

#define BLOCO_TAMANHO_SPRITE 128
#define BLOCO_TAMANHO_SPRITE_REDUZIDA 48

#define MAPA_LARGURA_PX (MAPA_LARGURA_BLOCOS * BLOCO_TAMANHO_SPRITE_REDUZIDA)
#define MAPA_ALTURA_PX (MAPA_ALTURA_BLOCOS * BLOCO_TAMANHO_SPRITE_REDUZIDA)

#define BLOCO_GRAMA       'G'
#define BLOCO_GRAMA_QUINA 'Q'
#define BLOCO_TERRA       'T'
#define BLOCO_TERRA_AGUA  't'
#define BLOCO_AGUA        'A'
#define BLOCO_AGUA_QUINA  'q'
#define BLOCO_ARVORE      'a'

typedef struct Map {
  ALLEGRO_BITMAP *background;
  char ***tiles;
} Map;

Map *iniciar_mapa(ALLEGRO_DISPLAY *display, const char *arquivo_mapa);

char ***criar_matriz_de_codigos_de_blocos();

void finalizar_matriz_de_codigos_de_blocos(char ***matriz);

void criar_cenario(ALLEGRO_BITMAP *background_sprites, Map *m, const char *arquivo_mapa);

void desenhar_tile(ALLEGRO_BITMAP *background_sprites, const char *tipo_do_bloco, int dx, int dy);

int *mapear_codigo_para_bloco(const char *s);

bool bloco_andavel(const char *bloco);

char *pegar_bloco_da_posicao(Map *m, int x, int y);

void finalizar_mapa(Map *m);