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
  Função que cria um novo jogador, inicializando-o com vários
  valores padrão. Retorna um ponteiro para a struct Player.
*/

Entity *criar_objetivo() {
  /* Aloca dinamicamente uma nova struct Player e inicializa-a. */
  Entity *p = criar_entidade();

  p->x = 2 * ENTITY_TAMANHO_SPRITE_REDUZIDA;
  p->y = 8 * ENTITY_TAMANHO_SPRITE_REDUZIDA;

  p->imagem = al_load_bitmap("assets/chest-sprite.png");
  if (!p->imagem) {
    puts("Não foi possível carregar a sprite do baú. Reinicie o jogo e tente novamente.");
  }
  assert(p->imagem != NULL);

  p->velocidade = 0;
  p->is_goal = true;

  return p;
}
