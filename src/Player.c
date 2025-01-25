#define ALLEGRO_STATICLINK

#include <Entity.h>
#include <Player.h>

/*
  Função que cria um novo jogador. Retorna um ponteiro para uma struct Entity.
*/

Entity *criar_player() {
  /* Aloca dinamicamente uma nova struct Entity e inicializa-a. */
  Entity *p = criar_entidade();

  p->velocidade = PLAYER_VELOCIDADE;
  p->is_player = true;

  return p;
}
