#pragma once

#include <allegro5/allegro5.h>
#include <Mapa.h>

#define ENTITY_DIRECAO_BAIXO     0
#define ENTITY_DIRECAO_ESQUERDA  1
#define ENTITY_DIRECAO_CIMA      2
#define ENTITY_DIRECAO_DIREITA   3

#define ENTITY_VELOCIDADE               1

#define ENTITY_TAMANHO_SPRITE          32
#define ENTITY_TAMANHO_SPRITE_REDUZIDA 52

typedef struct Entity {
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
  bool is_player;
  bool is_goal;
} Entity;

Entity *criar_entidade();

void parar_entidade(Entity *e);

void mudar_frame(Entity *e);

void verificar_movimento(Map *m, Entity *e);

void mover_entidade(Entity *e, int direcao, Map *m);

void inverte_direcao_entidade(Entity *e);

bool colidiu(Entity *entity1, Entity *entity2);

void finalizar_entidade(Entity *p);