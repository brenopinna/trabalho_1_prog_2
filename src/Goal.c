#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>
#include <Mapa.h>
#include <Player.h>
#include <Goal.h>

/*
  Função que cria o baú, que é o objetivo do jogo. Retorna um ponteiro para uma struct Entity.
*/

Entity *criar_objetivo() {
  /* Aloca dinamicamente uma nova struct Entity e inicializa-a. */
  Entity *p = criar_entidade();

  p->x = GOAL_POSICAO_X;
  p->y = GOAL_POSICAO_Y;

  p->imagem = al_load_bitmap("assets/chest-sprite.png");
  if (!p->imagem) {
    puts("Nao foi possivel abrir a imagem do sprite do bau. Reinicie o jogo e tente novamente.");
  }
  assert(p->imagem != NULL);

  p->velocidade = 0;
  p->is_goal = true;

  return p;
}
