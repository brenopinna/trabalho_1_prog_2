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

Player *criar_player() {
  /* Aloca dinamicamente uma nova struct Player e inicializa-a. */
  Player *p = malloc(sizeof(Player));

  p->imagem = al_load_bitmap("assets/player-sprites.png"); // Carrega no Allegro a imagem dos sprites do jogador.

  /* Caso o jogo não consiga abrir a imagem, uma mensagem
     de erro é exibida e o jogo encerra abruptamente. */
  if (p->imagem == NULL) {
    puts("Nao foi possivel abrir a imagem dos sprites do jogador. Reinicie o jogo e tente novamente.");
  }
  assert(p->imagem != NULL);

  p->x = PLAYER_TAMANHO_SPRITE_REDUZIDA; // Esta linha e a próxima colocam o jogador na posição padrão, que
  p->y = PLAYER_TAMANHO_SPRITE_REDUZIDA; // é no canto superior esquerdo da tela com uma pequena margem.
  p->velocidade = PLAYER_VELOCIDADE; // Define a velocidade do jogador.
  p->andando = false; // Por padrão, o jogador está parado.
  p->direcao = PLAYER_DIRECAO_BAIXO; // Por padrão, o jogador está virado para baixo.
  p->frame = 0; // Quando o jogador está parado, o seu sprite usa o quadro 0 da animação.
  p->inverte_frame = false; // A animação do sprite do jogador inicia no sentido normal.

  return p;
}

/*
  Função que indica que o jogador parou de andar e reinicia a sua animação.
*/

void parar_player(Player *p) {
  p->andando = false;
  p->frame = 0;
  p->inverte_frame = false;
}

/*
  Função que controla a animação do sprite do jogador. A animação
  é um loop de ida e volta entre os quadros 1, 2, 3 e 4.
*/

void mudar_frame(Player *p) {
  if (p->frame == 4) {
    p->inverte_frame = true;
  } else if (p->frame == 1) {
    p->inverte_frame = false;
  }
  p->frame += p->inverte_frame ? -1 : 1;
}

/*
  Função que detecta a colisão do personagem com os blocos não andáveis do
  cenário. Ela simula o movimento que o jogador faria, verifica se o jogador
  entraria em algum bloco do cenário em que ele não pode andar e salva na
  struct Player a permissão ou não de movimento para cada direção.
*/

void verificar_movimento(Map *m, Player *p) {
  /* Coleta a posição do jogador em uma variável local para comparação. */
  int x = p->x, y = p->y;

  /* Simula o movimento, alterando a posição do jogador de acordo
     com a tecla pressionada apenas nas variáveis locais. */
  if (p->direcao == PLAYER_DIRECAO_CIMA) { // Para cima.
    y -= p->velocidade;
  } if (p->direcao == PLAYER_DIRECAO_BAIXO) { // Para baixo.
    y += p->velocidade;
  } if (p->direcao == PLAYER_DIRECAO_DIREITA) { // Para a direita.
    x += p->velocidade;
  } if (p->direcao == PLAYER_DIRECAO_ESQUERDA) { // Para a esquerda.
    x -= p->velocidade;
  }

  /* Definição das margens de colisão do jogador. */
  int margem_right = PLAYER_TAMANHO_SPRITE_REDUZIDA;
  int margem_left = 0;
  int margem_bottom = PLAYER_TAMANHO_SPRITE_REDUZIDA;
  int margem_top = PLAYER_TAMANHO_SPRITE_REDUZIDA / 2;

  /* Verifica os blocos que estariam em cada um dos quatro cantos do sprite do
     jogador. Os códigos dos blocos são salvos em strings temporárias. */
  char *bloco_up_right = pegar_bloco_da_posicao(m, x + margem_right, y + margem_top); // Canto superior direito.
  char *bloco_up_left = pegar_bloco_da_posicao(m, x + margem_left, y + margem_top); // Canto superior esquerdo.
  char *bloco_bottom_right = pegar_bloco_da_posicao(m, x + margem_right, y + margem_bottom); // Canto inferior direito.
  char *bloco_bottom_left = pegar_bloco_da_posicao(m, x + margem_left, y + margem_bottom); // Canto inferior esquerdo.

  /* Verifica se cada um dos blocos encontrados anteriormente é andável
    ou não e usa isso para registrar na struct Player se o jogador pode
    se mover ou não para cada uma das quatro direções. */
  p->pode_mover_cima = bloco_andavel(bloco_up_right) && bloco_andavel(bloco_up_left); // Para cima.
  p->pode_mover_baixo = bloco_andavel(bloco_bottom_right) && bloco_andavel(bloco_bottom_left); // Para baixo.
  p->pode_mover_direita = bloco_andavel(bloco_up_right) && bloco_andavel(bloco_bottom_right); // Para a direita.
  p->pode_mover_esquerda = bloco_andavel(bloco_up_left) && bloco_andavel(bloco_bottom_left); // Para a esquerda.

  free(bloco_up_right);
  free(bloco_up_left);
  free(bloco_bottom_right);
  free(bloco_bottom_left);
}

/*
  Função que movimenta o jogador a cada quadro
  renderizado, de acordo com a sua velocidade.
*/

void mover_player(Player *p, int direcao, Map *m) {
  assert(0 <= direcao && direcao < 4); // O jogo encerra abruptamente se o código de direção for inválido.
  p->andando = true;
  p->direcao = direcao; // Muda a direção para a qual o jogador está virado.

  verificar_movimento(m, p); // Detecção de colisão.

  /* Altera a posição do jogador de acordo com a tecla pressionada, mas apenas se
     não houver sido detectada colisão. A lógica é semelhante à utilizada na função
     verificar_movimento, mas agora alterando os valores contidos na struct Player. */
  if (direcao == PLAYER_DIRECAO_CIMA && p->pode_mover_cima) {
    p->y -= p->velocidade;
  } else if (direcao == PLAYER_DIRECAO_BAIXO && p->pode_mover_baixo) {
    p->y += p->velocidade;
  } else if (direcao == PLAYER_DIRECAO_DIREITA && p->pode_mover_direita) {
    p->x += p->velocidade;
  } else if (direcao == PLAYER_DIRECAO_ESQUERDA && p->pode_mover_esquerda) {
    p->x -= p->velocidade;
  }
}

/*
  Função que libera a memória alocada para a struct Player.
*/

void finalizar_player(Player *p) {
  al_destroy_bitmap(p->imagem); // Libera a memória da imagem dos sprites do jogador.
  free(p); // Libera a memória da struct Player em si.
}
