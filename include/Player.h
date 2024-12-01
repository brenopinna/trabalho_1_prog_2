#pragma once

#include <allegro5/allegro5.h>
#include <Mapa.h>

#define PLAYER_DIRECTION_DOWN      0
#define PLAYER_DIRECTION_LEFT      1
#define PLAYER_DIRECTION_UP        2
#define PLAYER_DIRECTION_RIGHT     3

#define PLAYER_SPEED               2

#define PLAYER_SPRITE_SIZE        32
#define PLAYER_SCALED_SPRITE_SIZE 52

typedef struct Player {
  ALLEGRO_BITMAP *image;
  int x;
  int y;
  int velocidade;
  int andando;
  int direction;
  int frame;
  bool can_move_up;
  bool can_move_down;
  bool can_move_left;
  bool can_move_right;
} Player;

Player *criar_player();

void parar_player(Player *P);

void muda_frame(Player *P);

void verifica_movimento(Map *M, Player *P);

void move_player(Player *P, int direction, Map *M);

void finalizar_player(Player *P);