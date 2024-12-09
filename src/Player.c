#include <stdbool.h>
#include <assert.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>
#include <Player.h>
#include <Mapa.h>

/*
  Função que cria um novo jogador, inicializando-o com vários
  valores padrão. Retorna um ponteiro para a struct Player.
*/

Player *criar_player() {
  /* Aloca uma nova struct Player dinamicamente. */
  Player *P = malloc(sizeof(Player));

  P->image = al_load_bitmap("assets/player-sprites.png"); // Carrega no Allegro a imagem contendo os sprites do jogador.
  P->x = PLAYER_SCALED_SPRITE_SIZE; // Esta linha e a próxima colocam o jogador na posição padrão, que
  P->y = PLAYER_SCALED_SPRITE_SIZE; // é no canto superior esquerdo da tela com uma pequena margem.
  P->velocidade = PLAYER_SPEED; // Define a velocidade do jogador.
  P->andando = false; // Por padrão, o jogador está parado.
  P->direction = PLAYER_DIRECTION_DOWN; // Por padrão, o jogador está virado para baixo.
  P->frame = 0; // Quando o jogador está parado, o seu sprite usa o quadro 0 da animação.
  P->revert_frame = false; // A animação do sprite do jogador inicia no sentido normal.

  return P;
}

/*
  Função que indica que o jogador parou de andar e reinicia a sua animação.
*/

void parar_player(Player *P) {
  P->andando = false;
  P->frame = 0;
  P->revert_frame = false;
}

/*
  Função que controla a animação do sprite do jogador. A animação
  é um loop de ida e volta entre os quadros 1, 2, 3 e 4.
*/

void muda_frame(Player *P) {
  if (P->frame == 4) {
    P->revert_frame = true;
  } else if (P->frame == 1) {
    P->revert_frame = false;
  }
  P->frame += P->revert_frame ? -1 : 1;
}

/*
  Função que detecta a colisão do personagem com os blocos não andáveis do
  cenário. Ela simula o movimento que o jogador faria, verifica se o jogador
  entraria em algum bloco do cenário em que ele não pode andar e salva na
  struct Player a permissão ou não de movimento para cada direção.
*/

void verifica_movimento(Map *M, Player *P) {
  /* Coleta a posição do jogador em uma variável local para comparação. */
  int x = P->x, y = P->y;

  /* Simula o movimento, alterando a posição do jogador de acordo
     com a tecla pressionada apenas nas variáveis locais. */
  if (P->direction == PLAYER_DIRECTION_UP) { // Para cima.
    y -= P->velocidade;
  } if (P->direction == PLAYER_DIRECTION_DOWN) { // Para baixo.
    y += P->velocidade;
  } if (P->direction == PLAYER_DIRECTION_RIGHT) { // Para a direita.
    x += P->velocidade;
  } if (P->direction == PLAYER_DIRECTION_LEFT) { // Para a esquerda.
    x -= P->velocidade;
  }

  /* Definição das margens de colisão do jogador. */
  int margem_right = PLAYER_SCALED_SPRITE_SIZE;
  int margem_left = 0;
  int margem_bottom = PLAYER_SCALED_SPRITE_SIZE;
  int margem_top = PLAYER_SCALED_SPRITE_SIZE / 2;

  /* Verifica os blocos que estariam em cada um dos quatro cantos do sprite do
     jogador, e salva os códigos dos blocos em strings alocadas dinamicamente. */
  char *bloco_up_right = get_block_from_position(M, x + margem_right, y + margem_top); // Canto superior direito.
  char *bloco_up_left = get_block_from_position(M, x + margem_left, y + margem_top); // Canto superior esquerdo.
  char *bloco_bottom_right = get_block_from_position(M, x + margem_right, y + margem_bottom); // Canto inferior direito.
  char *bloco_bottom_left = get_block_from_position(M, x + margem_left, y + margem_bottom); // Canto inferior esquerdo.

  /* Verifica se cada um dos blocos encontrados anteriormente é andável
    ou não e usa isso para registrar na struct Player se o jogador pode
    se mover ou não para cada uma das quatro direções. */
  P->can_move_up = bloco_andavel(bloco_up_right) && bloco_andavel(bloco_up_left); // Para cima.
  P->can_move_down = bloco_andavel(bloco_bottom_right) && bloco_andavel(bloco_bottom_left); // Para baixo.
  P->can_move_right = bloco_andavel(bloco_up_right) && bloco_andavel(bloco_bottom_right); // Para a direita.
  P->can_move_left = bloco_andavel(bloco_up_left) && bloco_andavel(bloco_bottom_left); // Para a esquerda.

  /* Liberando a memória das strings. */
  free(bloco_up_right);
  free(bloco_up_left);
  free(bloco_bottom_right);
  free(bloco_bottom_left);
}

/*
  Função que movimenta o jogador a cada quadro
  renderizado, de acordo com a sua velocidade.
*/

void move_player(Player *P, int direction, Map *M) {
  assert(0 <= direction && direction < 4); // O jogo encerra abruptamente se o código de direção for inválido.
  P->andando = true;
  P->direction = direction; // Muda a direção para a qual o jogador está virado.

  verifica_movimento(M, P); // Detecção de colisão.

  /* Altera a posição do jogador de acordo com a tecla pressionada, mas apenas se
     não houver sido detectada colisão. A lógica é semelhante à utilizada na função
     verifica_movimento, mas agora alterando os valores contidos na struct Player. */
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

/*
  Função que libera a memória alocada para a struct Player.
*/

void finalizar_player(Player *P) {
  al_destroy_bitmap(P->image); // Libera a memória da imagem dos sprites do jogador.
  free(P); // Libera a memória da struct Player em si.
}
