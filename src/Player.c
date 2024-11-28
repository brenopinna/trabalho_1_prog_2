#include <stdbool.h>
#include <assert.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>
#include <Player.h>

Player *criar_player() {
  Player *P = malloc(sizeof(Player));

  P->image = al_load_bitmap("assets/player-sprites.png");
  P->x = 0;
  P->y = 0;
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

void move_player(Player *P, int direction) {
  assert(0 <= direction && direction < 4);
  P->andando = true;
  P->direction = direction;
  if (direction == PLAYER_DIRECTION_UP) {
    P->y -= P->velocidade;
  } else if (direction == PLAYER_DIRECTION_DOWN) {
    P->y += P->velocidade;
  } else if (direction == PLAYER_DIRECTION_RIGHT) {
    P->x += P->velocidade;
  } else if (direction == PLAYER_DIRECTION_LEFT) {
    P->x -= P->velocidade;
  }
}

void finalizar_player(Player *P) {
  al_destroy_bitmap(P->image);
  free(P);
}
