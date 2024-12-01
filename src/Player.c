#include <stdbool.h>
#include <assert.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>
#include <Player.h>
#include <Mapa.h>

Player *criar_player() {
  Player *P = malloc(sizeof(Player));

  P->image = al_load_bitmap("assets/player-sprites.png");
  P->x = PLAYER_SCALED_SPRITE_SIZE;
  P->y = PLAYER_SCALED_SPRITE_SIZE;
  P->velocidade = PLAYER_SPEED;
  P->andando = false;
  P->direction = PLAYER_DIRECTION_DOWN;
  P->frame = 0;

  return P;
};

void parar_player(Player *P) {
  P->andando = false;
  P->frame = 0;
}

void muda_frame(Player *P) {
  P->frame++;
  if (P->frame > 4) {
    P->frame = 0;
  }
}

void verifica_movimento(Map *M, Player *P) {
  int x = P->x, y = P->y;

  if (P->direction == PLAYER_DIRECTION_UP) {
    y -= P->velocidade;
  } if (P->direction == PLAYER_DIRECTION_DOWN) {
    y += P->velocidade;
  }  if (P->direction == PLAYER_DIRECTION_RIGHT) {
    x += P->velocidade;
  }  if (P->direction == PLAYER_DIRECTION_LEFT) {
    x -= P->velocidade;
  }

  int margem_right = PLAYER_SCALED_SPRITE_SIZE;
  int margem_left = 0;
  int margem_bottom = PLAYER_SCALED_SPRITE_SIZE;
  int margem_top = PLAYER_SCALED_SPRITE_SIZE / 2;

  char *bloco_up_right = get_block_from_position(M, x + margem_right, y + margem_top);
  char *bloco_up_left = get_block_from_position(M, x + margem_left, y + margem_top);
  char *bloco_bottom_right = get_block_from_position(M, x + margem_right, y + margem_bottom);
  char *bloco_bottom_left = get_block_from_position(M, x + margem_left, y + margem_bottom);
  // puts("AQUI");

  P->can_move_up = bloco_andavel(bloco_up_right) && bloco_andavel(bloco_up_left);
  P->can_move_down = bloco_andavel(bloco_bottom_right) && bloco_andavel(bloco_bottom_left);
  P->can_move_left = bloco_andavel(bloco_up_left) && bloco_andavel(bloco_bottom_left);
  P->can_move_right = bloco_andavel(bloco_up_right) && bloco_andavel(bloco_bottom_right);

  // puts("AQUI TAMBEM");
  free(bloco_up_right);
  free(bloco_up_left);
  free(bloco_bottom_right);
  free(bloco_bottom_left);
}

void move_player(Player *P, int direction, Map *M) {
  assert(0 <= direction && direction < 4);
  P->andando = true;
  P->direction = direction;

  verifica_movimento(M, P);

  if (direction == PLAYER_DIRECTION_UP && P->can_move_up) {
    P->y -= P->velocidade;
  } else if (direction == PLAYER_DIRECTION_DOWN && P->can_move_down) {
    P->y += P->velocidade;
  } else if (direction == PLAYER_DIRECTION_RIGHT && P->can_move_right) {
    P->x += P->velocidade;
  } else if (direction == PLAYER_DIRECTION_LEFT && P->can_move_left) {
    P->x -= P->velocidade;
  }
}

void finalizar_player(Player *P) {
  al_destroy_bitmap(P->image);
  free(P);
}
