#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>
#include <Mapa.h>
#include <Player.h>

/*
  Função que cria um novo jogador, inicializando-o com vários
  valores padrão. Retorna um ponteiro para a struct Player.
*/

Entity *criar_player() {
  /* Aloca dinamicamente uma nova struct Player e inicializa-a. */
  Entity *p = criar_entidade();

  p->velocidade = PLAYER_VELOCIDADE;
  p->is_player = true;

  return p;
}

/*
  Função que indica que o jogador parou de andar e reinicia a sua animação.
*/

/*
  Função que controla a animação do sprite do jogador. A animação
  é um loop de ida e volta entre os quadros 1, 2, 3 e 4.
*/

/*
void mudar_frame(Player *p) {
  if (p->frame == 4) {
    p->inverte_frame = true;
  } else if (p->frame == 1) {
    p->inverte_frame = false;
  }
  p->frame += p->inverte_frame ? -1 : 1;
}
*/

/*
  Função que detecta a colisão do personagem com os blocos não andáveis do
  cenário. Ela simula o movimento que o jogador faria, verifica se o jogador
  entraria em algum bloco do cenário em que ele não pode andar e salva na
  struct Player a permissão ou não de movimento para cada direção.
*/

/*
  Função que movimenta o jogador a cada quadro
  renderizado, de acordo com a sua velocidade.
*/

/*
  Função que libera a memória alocada para a struct Player.
*/

