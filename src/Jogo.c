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
  // Dados usados internamente pelo Allegro..
  ALLEGRO_DISPLAY *disp; // Janela.
  ALLEGRO_TIMER *timer; // Temporizador.
  ALLEGRO_EVENT_QUEUE *queue; // Fila de eventos.
  ALLEGRO_EVENT event; // Evento.
  ALLEGRO_FONT *f;

  Player *player;  // Ponteiro para uma struct Player.
  Map **mapas; // Vetor de ponteiros para structs Mapa. Mantém a lista dos mapas do jogo.
  int mapa; // Mantém registrado qual mapa está aberto no momento.
  bool *keys; // Vetor que armazena o estado das teclas do teclado.
  int frame_count; // Contagem de quadros renderizados. Usada para controlar o timing da animação do jogador.
};

//TODO: ver uma forma de renderizar a arvore em somente 1 bloco
/*
TODO: precisa colidir com a parte debaixo e passar atras da parte de cima
TODO: tem que ter uma renderizacao diferenciada por isso
*/

/*
  Função que faz a configuração inicial de um novo jogo.
  Retorna um ponteiro para uma struct Jogo alocada dinamicamente.
*/

Jogo *novo_jogo() {
  //TODO: Remover fontes, usei so pra testar.

  /* Incialização dos sistemas do Allegro necessários. */
  al_init();
  al_init_image_addon();
  al_install_keyboard();
  al_init_font_addon();

  /* Alocando uma nova struct Jogo dinamicamente e incicializando-a. */
  Jogo *J = malloc(sizeof(Jogo));

  // Dados usados internamente pelo Allegro.
  J->f = al_create_builtin_font();
  J->disp = al_create_display(MAP_PX_WIDTH, MAP_PX_HEIGHT);
  J->timer = al_create_timer(1.0 / 120.0);
  J->queue = al_create_event_queue();

  // Dados usados pelo jogo em si.
  J->keys = calloc(ALLEGRO_KEY_MAX, sizeof(bool)); // Inicializando o vetor do teclado com 0s.
  J->player = criar_player(); // Função definida em Player.c.
  J->mapas = malloc(2 * sizeof(Map *)); // Reserva espaço para dois mapas.
  J->mapas[0] = init_map(J->disp, "map_1.txt"); // Carrega, inicialmente, o Mapa 1. Função definida em Mapa.c.
  J->mapa = 1; // Registra que o mapa carregado foi o primeiro.
  J->frame_count = 0; // Inicia a contagem de quadros em 0.

  /* Registrando as fontes de eventos utilizadas. */
  al_register_event_source(J->queue, al_get_keyboard_event_source());
  al_register_event_source(J->queue, al_get_display_event_source(J->disp));
  al_register_event_source(J->queue, al_get_timer_event_source(J->timer));

  /* Inicia o temporizador. */
  al_start_timer(J->timer);

  return J;
}

/*
  Função que verifica se o jogo ainda está rodando. O jogo só será
  encerrado quando o usuário apertar a tecla ESC ou fechar a janela.
*/

bool jogo_rodando(Jogo *J) {
  if (J->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ||
      (J->event.type == ALLEGRO_EVENT_KEY_DOWN && J->keys[ALLEGRO_KEY_ESCAPE]))
    return false;

  return true;
}

/*
  Função que atualiza o jogo. Ela é chamada em loop pela função main
  até que o jogo seja fechado.
*/

void atualizar_jogo(Jogo *J) {
  al_wait_for_event(J->queue, &J->event);

  if (J->event.type == ALLEGRO_EVENT_KEY_DOWN) {
    J->keys[J->event.keyboard.keycode] = true;
  } else if (J->event.type == ALLEGRO_EVENT_KEY_UP) {
    J->keys[J->event.keyboard.keycode] = false;
    parar_player(J->player);
  }

  Player *player = J->player;
  const bool *keys = J->keys;

  if (J->event.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(J->queue)) {
    J->frame_count++;
    bool troca_mapa = false;

    if (J->frame_count >= 10 && player->andando) {
      muda_frame(J->player);
      J->frame_count = 0;
    }

    Map *mapa_atual = J->mapas[J->mapa - 1];

    if (keys[ALLEGRO_KEY_UP]) {
      move_player(J->player, PLAYER_DIRECTION_UP, mapa_atual);
    } else if (keys[ALLEGRO_KEY_DOWN]) {
      move_player(J->player, PLAYER_DIRECTION_DOWN, mapa_atual);
    } else if (keys[ALLEGRO_KEY_RIGHT]) {
      move_player(J->player, PLAYER_DIRECTION_RIGHT, mapa_atual);
    } else if (keys[ALLEGRO_KEY_LEFT]) {
      move_player(J->player, PLAYER_DIRECTION_LEFT, mapa_atual);
    } else if (keys[ALLEGRO_KEY_1]) {
      if (J->mapa != 1)
        troca_mapa = true;
    } else if (keys[ALLEGRO_KEY_2]) {
      if (J->mapa != 2)
        troca_mapa = true;
    }

    // TODO: isolar essa parte de trocar mapa em outra
    // TODO: e refatorar ela

    if (J->mapa == 1 && (J->player->x >= MAP_PX_WIDTH - PLAYER_SCALED_SPRITE_SIZE || troca_mapa)) {
      J->mapa = 2;
      finalizar_mapa(mapa_atual);
      mapa_atual = init_map(J->disp, "map_2.txt");
      J->mapas[1] = mapa_atual;
      J->player->x = 1;
    } else if (J->mapa == 2 && (J->player->x <= 0 || troca_mapa)) {
      J->mapa = 1;
      finalizar_mapa(mapa_atual);
      mapa_atual = init_map(J->disp, "map_1.txt");
      J->mapas[0] = mapa_atual;
      J->player->x = MAP_PX_WIDTH - PLAYER_SCALED_SPRITE_SIZE - 1;
    }

    if (troca_mapa) {
      J->player->x = PLAYER_SCALED_SPRITE_SIZE;
      J->player->y = PLAYER_SCALED_SPRITE_SIZE;
      J->player->direction = PLAYER_DIRECTION_DOWN;
    }

    al_draw_bitmap(mapa_atual->background, 0, 0, 0);
    al_draw_scaled_bitmap(player->image, player->frame * PLAYER_SPRITE_SIZE, player->direction * PLAYER_SPRITE_SIZE, PLAYER_SPRITE_SIZE, PLAYER_SPRITE_SIZE, player->x, player->y, PLAYER_SCALED_SPRITE_SIZE, PLAYER_SCALED_SPRITE_SIZE, 0);

    // TODO: Remover esa exibicao de texto, to usando so pra debugar
    char s[200];

    sprintf(s, "x: %d, y: %d\nUPRIGHT: %s\nUPLEFT: %s\nBOTTOMRIGHT: %s\nBOTTOMLEFT: %s\n",
            J->player->x, J->player->y,
            get_block_from_position(mapa_atual, J->player->x + PLAYER_SCALED_SPRITE_SIZE, J->player->y),
            get_block_from_position(mapa_atual, J->player->x, J->player->y),
            get_block_from_position(mapa_atual, J->player->x + PLAYER_SCALED_SPRITE_SIZE, J->player->y + PLAYER_SCALED_SPRITE_SIZE),
            get_block_from_position(mapa_atual, J->player->x, J->player->y + PLAYER_SCALED_SPRITE_SIZE)
    );

    al_draw_multiline_text(J->f, al_map_rgb(0, 0, 0), 10, 10, 200, 10, 0, s);

    al_flip_display();
  }
}

void finalizar_jogo(Jogo *J) {
  al_destroy_display(J->disp);
  al_destroy_timer(J->timer);
  al_destroy_event_queue(J->queue);
  free(J->keys);
  al_destroy_font(J->f);
  finalizar_player(J->player);
  finalizar_mapa(J->mapas[J->mapa - 1]);
  free(J->mapas);
  al_uninstall_system();
  free(J);
}
