#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>
#include <Mapa.h>
#include <Entity.h>

/*
  Função que cria um novo jogador, inicializando-o com vários
  valores padrão. Retorna um ponteiro para a struct Entity.
*/

Entity *criar_entidade() {
  /* Aloca dinamicamente uma nova struct Entity e inicializa-a. */
  Entity *e = malloc(sizeof(Entity));

  e->imagem = al_load_bitmap("assets/player-sprites.png"); // Carrega no Allegro a imagem dos sprites do jogador.

  /* Caso o jogo não consiga abrir a imagem, uma mensagem
     de erro é exibida e o jogo encerra abruptamente. */
  if (e->imagem == NULL) {
    puts("Nao foi possivel abrir a imagem dos sprites da entidade. Reinicie o jogo e tente novamente.");
  }
  assert(e->imagem != NULL);

  e->x = ENTITY_TAMANHO_SPRITE_REDUZIDA; // Esta linha e a próxima colocam o jogador na posição padrão, que
  e->y = ENTITY_TAMANHO_SPRITE_REDUZIDA; // é no canto superior esquerdo da tela com uma pequena margem.
  e->velocidade = ENTITY_VELOCIDADE; // Define a velocidade do jogador.
  e->andando = false; // Por padrão, o jogador está parado.
  e->direcao = ENTITY_DIRECAO_BAIXO; // Por padrão, o jogador está virado para baixo.
  e->frame = 0; // Quando o jogador está parado, o seu sprite usa o quadro 0 da animação.
  e->inverte_frame = false; // A animação do sprite do jogador inicia no sentido normal.
  e->is_player = false;

  return e;
}

/*
  Função que indica que o jogador parou de andar e reinicia a sua animação.
*/

void parar_entidade(Entity *e) {
  e->andando = false;
  e->frame = 0;
  e->inverte_frame = false;
}

/*
  Função que controla a animação do sprite do jogador. A animação
  é um loop de ida e volta entre os quadros 1, 2, 3 e 4.
*/

void mudar_frame(Entity *e) {
  if (e->frame == 4) {
    e->inverte_frame = true;
  } else if (e->frame == 1) {
    e->inverte_frame = false;
  }
  e->frame += e->inverte_frame ? -1 : 1;
}

/*
  Função que detecta a colisão do personagem com os blocos não andáveis do
  cenário. Ela simula o movimento que o jogador faria, verifica se o jogador
  entraria em algum bloco do cenário em que ele não pode andar e salva na
  struct Entity a permissão ou não de movimento para cada direção.
*/

void verificar_movimento(Map *m, Entity *e) {
  /* Coleta a posição do jogador em uma variável local para comparação. */
  int x = e->x, y = e->y;

  /* Simula o movimento, alterando a posição do jogador de acordo
     com a tecla pressionada apenas nas variáveis locais. */
  if (e->direcao == ENTITY_DIRECAO_CIMA) { // Para cima.
    y -= e->velocidade;
  } if (e->direcao == ENTITY_DIRECAO_BAIXO) { // Para baixo.
    y += e->velocidade;
  } if (e->direcao == ENTITY_DIRECAO_DIREITA) { // Para a direita.
    x += e->velocidade;
  } if (e->direcao == ENTITY_DIRECAO_ESQUERDA) { // Para a esquerda.
    x -= e->velocidade;
  }

  /* Definição das margens de colisão do jogador. */
  int margem_right = ENTITY_TAMANHO_SPRITE_REDUZIDA;
  int margem_left = 0;
  int margem_bottom = ENTITY_TAMANHO_SPRITE_REDUZIDA;
  int margem_top = ENTITY_TAMANHO_SPRITE_REDUZIDA / 2;

  /* Verifica os blocos que estariam em cada um dos quatro cantos do sprite do
     jogador. Os códigos dos blocos são salvos em strings temporárias. */
  char *bloco_up_right = pegar_bloco_da_posicao(m, x + margem_right, y + margem_top); // Canto superior direito.
  char *bloco_up_left = pegar_bloco_da_posicao(m, x + margem_left, y + margem_top); // Canto superior esquerdo.
  char *bloco_bottom_right = pegar_bloco_da_posicao(m, x + margem_right, y + margem_bottom); // Canto inferior direito.
  char *bloco_bottom_left = pegar_bloco_da_posicao(m, x + margem_left, y + margem_bottom); // Canto inferior esquerdo.

  /* Verifica se cada um dos blocos encontrados anteriormente é andável
    ou não e usa isso para registrar na struct Entity se o jogador pode
    se mover ou não para cada uma das quatro direções. */
  e->pode_mover_cima = bloco_andavel(bloco_up_right) && bloco_andavel(bloco_up_left); // Para cima.
  e->pode_mover_baixo = bloco_andavel(bloco_bottom_right) && bloco_andavel(bloco_bottom_left); // Para baixo.
  e->pode_mover_direita = bloco_andavel(bloco_up_right) && bloco_andavel(bloco_bottom_right); // Para a direita.
  e->pode_mover_esquerda = bloco_andavel(bloco_up_left) && bloco_andavel(bloco_bottom_left); // Para a esquerda.

  free(bloco_up_right);
  free(bloco_up_left);
  free(bloco_bottom_right);
  free(bloco_bottom_left);
}

/*
  Função que movimenta o jogador a cada quadro
  renderizado, de acordo com a sua velocidade.
*/

void mover_entidade(Entity *e, int direcao, Map *m) {
  assert(0 <= direcao && direcao < 4); // O jogo encerra abruptamente se o código de direção for inválido.
  e->andando = true;
  e->direcao = direcao; // Muda a direção para a qual o jogador está virado.

  verificar_movimento(m, e); // Detecção de colisão.

  if (!e->is_player) {
    inverte_direcao_entidade(e);
  }

  /* Altera a posição do jogador de acordo com a tecla pressionada, mas apenas se
     não houver sido detectada colisão. A lógica é semelhante à utilizada na função
     verificar_movimento, mas agora alterando os valores contidos na struct Entity. */
  if (direcao == ENTITY_DIRECAO_CIMA && e->pode_mover_cima) {
    e->y -= e->velocidade;
  } else if (direcao == ENTITY_DIRECAO_BAIXO && e->pode_mover_baixo) {
    e->y += e->velocidade;
  } else if (direcao == ENTITY_DIRECAO_DIREITA && e->pode_mover_direita) {
    e->x += e->velocidade;
  } else if (direcao == ENTITY_DIRECAO_ESQUERDA && e->pode_mover_esquerda) {
    e->x -= e->velocidade;
  }
}

void inverte_direcao_entidade(Entity *e) {
  if ((e->direcao == ENTITY_DIRECAO_CIMA && !e->pode_mover_cima) ||
      (e->direcao == ENTITY_DIRECAO_BAIXO && !e->pode_mover_baixo) ||
      (e->direcao == ENTITY_DIRECAO_DIREITA && !e->pode_mover_direita) ||
      (e->direcao == ENTITY_DIRECAO_ESQUERDA && !e->pode_mover_esquerda)) {
    e->direcao = (e->direcao + 2) % 4;
  }
}

/*
  Função que libera a memória alocada para a struct Entity.
*/

void finalizar_entidade(Entity *e) {
  al_destroy_bitmap(e->imagem); // Libera a memória da imagem dos sprites do jogador.
  free(e); // Libera a memória da struct Entity em si.
}
