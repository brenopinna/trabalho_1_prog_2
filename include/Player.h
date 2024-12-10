#pragma once

#include <allegro5/allegro5.h>
#include <Mapa.h>

#define PLAYER_DIRECAO_BAIXO     0
#define PLAYER_DIRECAO_ESQUERDA  1
#define PLAYER_DIRECAO_CIMA      2
#define PLAYER_DIRECAO_DIREITA   3

#define PLAYER_VELOCIDADE               2

#define PLAYER_TAMANHO_SPRITE          32
#define PLAYER_TAMANHO_SPRITE_REDUZIDA 52

typedef struct Player {
  ALLEGRO_BITMAP *imagem;
  int x;
  int y;
  int velocidade;
  int andando;
  int direcao;
  int frame;
  bool inverte_frame;
  bool pode_mover_cima;
  bool pode_mover_baixo;
  bool pode_mover_esquerda;
  bool pode_mover_direita;
} Player;

Player *criar_player();

void parar_player(Player *p);

void mudar_frame(Player *p);

void verificar_movimento(Map *m, Player *p);

void mover_player(Player *p, int direcao, Map *m);

void finalizar_player(Player *p);