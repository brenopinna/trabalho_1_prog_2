#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <Jogo.h>
#include <Player.h>
#include <Mapa.h>

/*
  Definição da struct Jogo. Ela armazena todas as informações
  necessárias para manter o jogo funcionando.
*/

struct Jogo {
  // Dados usados internamente pelo Allegro.
  ALLEGRO_DISPLAY *disp; // Janela.
  ALLEGRO_TIMER *timer; // Temporizador.
  ALLEGRO_EVENT_QUEUE *queue; // Fila de eventos.
  ALLEGRO_EVENT event; // Evento.
  ALLEGRO_FONT *f;

  Player *player;  // Ponteiro para uma struct Player.
  Map **mapas; // Vetor de ponteiros para structs Map. Mantém a lista dos mapas do jogo.
  int mapa; // Mantém registrado qual mapa está aberto no momento.
  bool *keys; // Vetor que armazena o estado das teclas do teclado.
  int frame_count; // Contagem de quadros renderizados. Usada para controlar a velocidade da animação do sprite do jogador.
};

// TODO: Ver uma forma de renderizar a árvore em somente um bloco.
// TODO: Precisa colidir com a parte debaixo e passar atrás da parte de cima.
// TODO: Tem que ter uma renderização diferenciada para isso.

/*
  Função que faz a configuração inicial de um novo jogo.
  Retorna um ponteiro para uma struct Jogo alocada dinamicamente.
*/

Jogo *novo_jogo() {
  // TODO: Remover fontes. Usei só pra testar.

  /* Inicialização dos sistemas do Allegro necessários. */
  al_init();
  al_init_image_addon();
  al_install_keyboard();
  al_init_font_addon();

  /* Aloca uma nova struct Jogo dinamicamente e inicializa-a. */
  Jogo *J = malloc(sizeof(Jogo));

  // Dados usados internamente pelo Allegro.
  J->disp = al_create_display(MAP_PX_WIDTH, MAP_PX_HEIGHT);
  J->timer = al_create_timer(1.0 / 120.0);
  J->queue = al_create_event_queue();
  J->f = al_create_builtin_font();

  // Dados usados pelo jogo em si.
  J->keys = calloc(ALLEGRO_KEY_MAX, sizeof(bool)); // Inicializa o vetor do teclado com zeros.
  J->player = criar_player(); // Função definida em Player.c.
  J->mapas = malloc(2 * sizeof(Map *)); // Reserva espaço para dois mapas.
  J->mapas[0] = init_map(J->disp, "map_1.txt"); // Carrega, inicialmente, o Mapa 1. Função definida em Mapa.c.
  J->mapa = 1; // Registra que o mapa carregado foi o primeiro.
  J->frame_count = 0; // Inicia a contagem de quadros em 0.

  /* Registra as fontes de eventos utilizadas. */
  al_register_event_source(J->queue, al_get_keyboard_event_source());
  al_register_event_source(J->queue, al_get_display_event_source(J->disp));
  al_register_event_source(J->queue, al_get_timer_event_source(J->timer));

  /* Inicia o temporizador. */
  al_start_timer(J->timer);

  return J;
}

/*
  Função que verifica se o jogo ainda está rodando e retorna
  verdadeiro ou falso. O jogo só é encerrado quando o
  usuário apertar a tecla ESC ou fechar a janela.
*/

bool jogo_rodando(Jogo *J) {
  if (J->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ||
      (J->event.type == ALLEGRO_EVENT_KEY_DOWN && J->keys[ALLEGRO_KEY_ESCAPE]))
    return false;

  return true;
}

/*
  Função que atualiza o jogo. Ela é chamada em loop
  pela função main até que o jogo seja fechado.
*/

void atualizar_jogo(Jogo *J) {
  /* Espera um evento acontecer. Assim que acontecer, salva as informações do
     evento em J->event, para que elas possam ser tratadas no resto da função. */
  al_wait_for_event(J->queue, &J->event);

  /* Verifica se uma tecla foi pressionada ou solta e registra no vetor de teclas. */
  if (J->event.type == ALLEGRO_EVENT_KEY_DOWN) {
    J->keys[J->event.keyboard.keycode] = true;
  } else if (J->event.type == ALLEGRO_EVENT_KEY_UP) {
    J->keys[J->event.keyboard.keycode] = false;
    parar_player(J->player); // Se qualquer tecla for solta, o jogador para.
  }

  /* Lógica principal da atualização do jogo. Só é executada em sincronia com
     o temporizador e quando não há mais outros eventos a serem processados. */
  if (J->event.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(J->queue)) {
    J->frame_count++; // Aumenta a contagem de quadros renderizados.
    bool troca_mapa = false; // Indica se o mapa foi trocado a pedido do usuário ou não.

    /* Controla a ocorrência e a velocidade da animação do sprite do
       jogador. A animação só ocorre quando o jogador está andando
       e avança um quadro a cada dez quadros renderizados. */
    if (J->frame_count >= 10 && J->player->andando) {
      muda_frame(J->player); // Função definida em Player.c.
      J->frame_count = 0; // Reinicia a contagem de quadros renderizados.
    }

    /* Verifica se o usuário deu algum comando de teclado válido
       e executa a ação correspondente ao comando detectado. */

    /* Movimento do jogador. Função move_player definida em Player.c. */
    if (J->keys[ALLEGRO_KEY_UP]) { // Para cima.
      move_player(J->player, PLAYER_DIRECTION_UP, J->mapas[J->mapa - 1]); // J->mapas[J->mapa - 1] é o mapa atual.
    } else if (J->keys[ALLEGRO_KEY_DOWN]) { // Para baixo.
      move_player(J->player, PLAYER_DIRECTION_DOWN, J->mapas[J->mapa - 1]);
    } else if (J->keys[ALLEGRO_KEY_RIGHT]) { // Para a direita.
      move_player(J->player, PLAYER_DIRECTION_RIGHT, J->mapas[J->mapa - 1]);
    } else if (J->keys[ALLEGRO_KEY_LEFT]) { // Para a esquerda.
      move_player(J->player, PLAYER_DIRECTION_LEFT, J->mapas[J->mapa - 1]);
    }

    /* Troca instantânea de mapa a pedido do usuário.
       Só ocorre se o mapa escolhido não for o atual. */
    else if (J->keys[ALLEGRO_KEY_1]) { // Mapa 1.
      if (J->mapa != 1)
        troca_mapa = true;
    } else if (J->keys[ALLEGRO_KEY_2]) { // Mapa 2.
      if (J->mapa != 2)
        troca_mapa = true;
    }

    // TODO: Isolar essa parte de trocar mapa em outra função e refatorá-la.

    /* Troca o mapa caso o jogador tenha passado dos limites do mapa (à
       direita e à esquerda, respectivamente) ou caso o usuário tenha
       feito a troca instantânea de mapa. Funções finalizar_mapa e
       init_map definidas em Mapa.c. */

    /* Troca do Mapa 1 para o Mapa 2. */
    if (J->mapa == 1 && (J->player->x >= MAP_PX_WIDTH - PLAYER_SCALED_SPRITE_SIZE || troca_mapa)) {
      finalizar_mapa(J->mapas[J->mapa - 1]);
      J->mapa = 2;
      J->mapas[J->mapa - 1] = init_map(J->disp, "map_2.txt");
      J->player->x = 1; // Teleporta o jogador para o lado esquerdo do mapa.
    }

    /* Troca do Mapa 2 para o Mapa 1. */
    else if (J->mapa == 2 && (J->player->x <= 0 || troca_mapa)) {
      finalizar_mapa(J->mapas[J->mapa - 1]);
      J->mapa = 1;
      J->mapas[J->mapa - 1] = init_map(J->disp, "map_1.txt");
      J->player->x = MAP_PX_WIDTH - PLAYER_SCALED_SPRITE_SIZE - 1; // Teleporta o jogador para o lado direito do mapa.
    }

    /* Reseta a posição do jogador para o padrão caso a
       troca de mapa tenha sido a pedido do usuário. */
    if (troca_mapa) {
      J->player->x = PLAYER_SCALED_SPRITE_SIZE;
      J->player->y = PLAYER_SCALED_SPRITE_SIZE;
      J->player->direction = PLAYER_DIRECTION_DOWN;
    }

    /* Renderiza tudo (finalmente!). */

    /* Desenha o cenário. */
    al_draw_bitmap(J->mapas[J->mapa - 1]->background, 0, 0, 0);

    /* Desenha o quadro certo do sprite do jogador com a direção, posição e tamanho corretos. */
    al_draw_scaled_bitmap(J->player->image, J->player->frame * PLAYER_SPRITE_SIZE, J->player->direction * PLAYER_SPRITE_SIZE,
        PLAYER_SPRITE_SIZE, PLAYER_SPRITE_SIZE, J->player->x, J->player->y, PLAYER_SCALED_SPRITE_SIZE, PLAYER_SCALED_SPRITE_SIZE, 0);

    // TODO: Remover essa exibição de texto. Estou usando só para debugging.
    char s[200];

    sprintf(s, "x: %d, y: %d\nUPRIGHT: %s\nUPLEFT: %s\nBOTTOMRIGHT: %s\nBOTTOMLEFT: %s\nPFRAME: %d\n",
            J->player->x, J->player->y,
            get_block_from_position(J->mapas[J->mapa - 1], J->player->x + PLAYER_SCALED_SPRITE_SIZE, J->player->y),
            get_block_from_position(J->mapas[J->mapa - 1], J->player->x, J->player->y),
            get_block_from_position(J->mapas[J->mapa - 1], J->player->x + PLAYER_SCALED_SPRITE_SIZE, J->player->y + PLAYER_SCALED_SPRITE_SIZE),
            get_block_from_position(J->mapas[J->mapa - 1], J->player->x, J->player->y + PLAYER_SCALED_SPRITE_SIZE),
            J->player->frame
    );

    al_draw_multiline_text(J->f, al_map_rgb(0, 0, 0), 10, 10, 200, 10, 0, s);

    /* Atualiza a tela com o novo quadro renderizado. */
    al_flip_display();
  }
}

/*
  Função que finaliza os sistemas do Allegro e
  libera a memória alocada para a struct Jogo.
*/

void finalizar_jogo(Jogo *J) {
  /* Libera a memória dos dados usados internamente pelo Allegro. */
  al_destroy_display(J->disp);
  al_destroy_timer(J->timer);
  al_destroy_event_queue(J->queue);
  al_destroy_font(J->f);

  /* Libera a memória alocada dos dados alocados
     dinamicamente que fazem parte da struct Jogo. */
  free(J->keys);
  finalizar_mapa(J->mapas[J->mapa - 1]); // Função definida em Mapa.c.
  free(J->mapas);
  finalizar_player(J->player); // Função definida em Player.c.

  /* Finalização definitiva do Allegro. */
  al_uninstall_system();

  /* Libera a memória da struct Jogo em si. */
  free(J);
}
