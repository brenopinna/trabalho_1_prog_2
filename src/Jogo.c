#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <Mapa.h>
#include <Player.h>
#include <Jogo.h>
#include <Entity.h>
#include <time.h>
#include <Texto.h>

/*
  Definição da struct Jogo. Ela armazena todas as informações
  necessárias para manter o jogo funcionando.
*/

struct Jogo {
  /* Dados usados internamente pelo Allegro. */
  ALLEGRO_DISPLAY *disp; // Janela.
  ALLEGRO_TIMER *timer; // Temporizador.
  ALLEGRO_EVENT_QUEUE *queue; // Fila de eventos.
  ALLEGRO_EVENT event; // Evento.
  ALLEGRO_FONT *font;
  Text *title;
  Text *subtitle;

  /* Dados usados pelo jogo em si. */
  Entity *player;  // Ponteiro para uma struct Player.
  Entity *enemy;
  Entity *enemy2;
  Map **mapas; // Vetor de ponteiros para structs Map. Mantém a lista dos mapas do jogo.
  int mapa; // Mantém registrado qual mapa está aberto no momento.
  bool *keys; // Vetor que armazena o estado das teclas do teclado.
  int frame_count; // Contagem de quadros renderizados. Usada para controlar a velocidade da animação do sprite do jogador.
  bool derrota;
  bool show_title;
};

// TODO: Ver uma forma de renderizar a árvore em somente um bloco.
// TODO: Precisa colidir com a parte debaixo e passar atrás da parte de cima.
// TODO: Tem que ter uma renderização diferenciada para isso.

/*
  Função que faz a configuração inicial de um novo jogo.
  Retorna um ponteiro para uma struct Jogo alocada dinamicamente.
*/

Jogo *novo_jogo() {
  srand(time(NULL));

  // TODO: Remover fontes. Usei só pra testar.

  /* Inicialização dos sistemas do Allegro necessários. */
  al_init();
  al_init_image_addon();
  al_install_keyboard();
  al_init_font_addon();

  /* Aloca dinamicamente uma nova struct Jogo e inicializa-a. */
  Jogo *J = malloc(sizeof(Jogo));

  // Dados usados internamente pelo Allegro.
  J->disp = al_create_display(MAPA_LARGURA_PX, MAPA_ALTURA_PX);
  J->timer = al_create_timer(1.0 / 120.0);
  J->queue = al_create_event_queue();
  J->font = al_create_builtin_font();
  J->title = create_text_bitmap(J->disp, "AS AVENTURAS DE JEMENELSON!", J->font, al_map_rgb(255, 255, 255));
  J->subtitle = create_text_bitmap(J->disp, "pressione qualquer tecla para iniciar.", J->font, al_map_rgb(255, 255, 255));

  // Dados usados pelo jogo em si.
  J->keys = calloc(ALLEGRO_KEY_MAX, sizeof(bool)); // Inicializa o vetor do teclado com zeros.
  J->player = criar_player(); // Função definida em Player.c.
  J->enemy = criar_entidade();
  J->enemy2 = criar_entidade();

  //! REMOVER ISSO AQ QUANDO OS MAPAS ESTIVEREM PRONTOS, EH SO PRA TESTAR A COLISAO
  J->enemy2->x = J->enemy->x = ENTITY_TAMANHO_SPRITE_REDUZIDA * 12;
  J->enemy2->y = J->enemy->y = ENTITY_TAMANHO_SPRITE_REDUZIDA * 3;
  //!
  J->mapas = malloc(2 * sizeof(Map *)); // Reserva espaço para dois mapas.
  J->mapas[0] = iniciar_mapa(J->disp, "map_1.txt"); // Carrega, inicialmente, o Mapa 1. Função definida em Mapa.c.
  J->mapa = 1; // Registra que o mapa carregado foi o primeiro.
  J->frame_count = 0; // Inicia a contagem de quadros em 0.
  J->derrota = false;
  J->show_title = true;

  /* Registra as fontes de eventos utilizadas. */
  al_register_event_source(J->queue, al_get_keyboard_event_source());
  al_register_event_source(J->queue, al_get_display_event_source(J->disp));
  al_register_event_source(J->queue, al_get_timer_event_source(J->timer));

  /* Inicia o temporizador. */
  al_start_timer(J->timer);

  return J;
}

/*
  Função que verifica se o jogo ainda está rodando. O jogo só é fechado quando
  o usuário aperta a tecla ESC ou fecha a janela, a não ser que seja encerrado
  abruptamente por algum assert. Retorna um valor booleano.
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
    if (J->show_title) J->show_title = false;
  } else if (J->event.type == ALLEGRO_EVENT_KEY_UP) {
    J->keys[J->event.keyboard.keycode] = false;
    parar_entidade(J->player); // Se qualquer tecla for solta, o jogador para.
  }

  /* Lógica principal da atualização do jogo. Só é executada em sincronia com
     o temporizador e quando não há mais outros eventos a serem processados. */
  if (J->event.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(J->queue)) {
    if (J->derrota) return;
    bool troca_mapa = false; // Indica se o mapa foi trocado a pedido do usuário ou não.
    if (!(J->frame_count % 10)) {
      if (!(J->frame_count % (70 + (rand() % 4) * 10))) {
        J->enemy->direcao = rand() % 4;
      }
      if (!(J->frame_count % (70 + (rand() % 4) * 10))) {
        J->enemy2->direcao = rand() % 4;
      }

      if (J->enemy->andando)
        mudar_frame(J->enemy); // Função definida em Player.c.
      if (J->enemy2->andando)
        mudar_frame(J->enemy2); // Função definida em Player.c.
      if (J->player->andando) {
        mudar_frame(J->player); // Função definida em Player.c.
      }
    }

    mover_entidade(J->enemy, J->enemy->direcao, J->mapas[J->mapa - 1]);
    mover_entidade(J->enemy2, J->enemy2->direcao, J->mapas[J->mapa - 1]);

    /* Verifica se o usuário deu algum comando de teclado válido
       e executa a ação correspondente ao comando detectado. */

       /* Movimento do jogador. Função mover_player definida em Player.c. */
    if (J->keys[ALLEGRO_KEY_UP]) { // Para cima.
      mover_entidade(J->player, ENTITY_DIRECAO_CIMA, J->mapas[J->mapa - 1]); // J->mapas[J->mapa - 1] é o mapa atual.
    } else if (J->keys[ALLEGRO_KEY_DOWN]) { // Para baixo.
      mover_entidade(J->player, ENTITY_DIRECAO_BAIXO, J->mapas[J->mapa - 1]);
    } else if (J->keys[ALLEGRO_KEY_RIGHT]) { // Para a direita.
      mover_entidade(J->player, ENTITY_DIRECAO_DIREITA, J->mapas[J->mapa - 1]);
    } else if (J->keys[ALLEGRO_KEY_LEFT]) { // Para a esquerda.
      mover_entidade(J->player, ENTITY_DIRECAO_ESQUERDA, J->mapas[J->mapa - 1]);
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

    if (colidiu(J->player, J->enemy) || colidiu(J->player, J->enemy2)) {
      J->derrota = true;
    }

    // TODO: Isolar essa parte de trocar mapa em outra função e refatorá-la.

    /* Troca o mapa caso o jogador tenha passado dos limites do mapa (à
       direita e à esquerda, respectivamente) ou caso o usuário tenha
       feito a troca instantânea de mapa. Funções finalizar_mapa e
       init_map definidas em Mapa.c. */

       /* Troca do Mapa 1 para o Mapa 2. */
    if (J->mapa == 1 && (J->player->x >= MAPA_LARGURA_PX - ENTITY_TAMANHO_SPRITE_REDUZIDA || troca_mapa)) {
      finalizar_mapa(J->mapas[J->mapa - 1]);
      J->mapa = 2;
      J->mapas[J->mapa - 1] = iniciar_mapa(J->disp, "map_2.txt");
      J->player->x = 1; // Teleporta o jogador para o lado esquerdo do mapa.
      //! REMOVER ISSO AQ QUANDO OS MAPAS ESTIVEREM PRONTOS, EH SO PRA TESTAR A COLISAO
      J->enemy2->x = J->enemy->x = ENTITY_TAMANHO_SPRITE_REDUZIDA * 12;
      J->enemy2->y = J->enemy->y = ENTITY_TAMANHO_SPRITE_REDUZIDA * 7;
      //!
    }

    /* Troca do Mapa 2 para o Mapa 1. */
    else if (J->mapa == 2 && (J->player->x <= 0 || troca_mapa)) {
      finalizar_mapa(J->mapas[J->mapa - 1]);
      J->mapa = 1;
      J->mapas[J->mapa - 1] = iniciar_mapa(J->disp, "map_1.txt");
      J->player->x = MAPA_LARGURA_PX - ENTITY_TAMANHO_SPRITE_REDUZIDA - 1; // Teleporta o jogador para o lado direito do mapa.
      //! REMOVER ISSO AQ QUANDO OS MAPAS ESTIVEREM PRONTOS, EH SO PRA TESTAR A COLISAO
      J->enemy2->x = J->enemy->x = ENTITY_TAMANHO_SPRITE_REDUZIDA * 12;
      J->enemy2->y = J->enemy->y = ENTITY_TAMANHO_SPRITE_REDUZIDA * 7;
      //!
    }

    /* Reseta a posição do jogador para o padrão caso a
       troca de mapa tenha sido a pedido do usuário. */
    if (troca_mapa) {
      J->player->x = ENTITY_TAMANHO_SPRITE_REDUZIDA;
      J->player->y = ENTITY_TAMANHO_SPRITE_REDUZIDA;
      J->player->direcao = ENTITY_DIRECAO_BAIXO;
      //! REMOVER ISSO AQ QUANDO OS MAPAS ESTIVEREM PRONTOS, EH SO PRA TESTAR A COLISAO
      J->enemy2->x = J->enemy->x = ENTITY_TAMANHO_SPRITE_REDUZIDA * 12;
      J->enemy2->y = J->enemy->y = ENTITY_TAMANHO_SPRITE_REDUZIDA * 7;
      //!
      J->frame_count = 0;
    }

    /* Lógica de renderização. */

    Entity *entities[] = { J->player, J->enemy, J->enemy2 };
    int size = sizeof(entities) / sizeof(Entity *);
    for (int i = 0; i < size; i++) {
      bool troca = false;
      for (int j = 0; j < size - 1; j++) {
        if (entities[j]->y >= entities[j + 1]->y) {
          Entity *tmp = entities[j];
          entities[j] = entities[j + 1];
          entities[j + 1] = tmp;
          troca = true;
        }
      }
      if (!troca) break;
    }


    if (J->show_title) {
      draw_centered_scaled_text(J->title, 3, 0, 0);
      draw_centered_scaled_text(J->subtitle, 1.6, 0, FONT_SIZE * 3);
    }

    if (!J->derrota) {
      // Desenha o cenário.
      al_draw_bitmap(J->mapas[J->mapa - 1]->background, 0, 0, 0);

      for (int i = 0; i < 3; i++) {
        if (entities[i]->is_player) {
          al_draw_scaled_bitmap(entities[i]->imagem, entities[i]->frame * ENTITY_TAMANHO_SPRITE, entities[i]->direcao * ENTITY_TAMANHO_SPRITE,
                                ENTITY_TAMANHO_SPRITE, ENTITY_TAMANHO_SPRITE, entities[i]->x, entities[i]->y, ENTITY_TAMANHO_SPRITE_REDUZIDA, ENTITY_TAMANHO_SPRITE_REDUZIDA, 0);
        } else {
          al_draw_tinted_scaled_bitmap(entities[i]->imagem, al_map_rgba(50, 100, 255, 150), entities[i]->frame * ENTITY_TAMANHO_SPRITE, entities[i]->direcao * ENTITY_TAMANHO_SPRITE,
                                       ENTITY_TAMANHO_SPRITE, ENTITY_TAMANHO_SPRITE, entities[i]->x, entities[i]->y, ENTITY_TAMANHO_SPRITE_REDUZIDA, ENTITY_TAMANHO_SPRITE_REDUZIDA, 0);
        }
      }

    } else {
      // TODO: Aqui, renderizar um texto de derrota
    }
    // Atualiza a tela com o novo quadro renderizado.
    al_flip_display();
    J->frame_count++; // Aumenta a contagem de quadros renderizados.
  }
}

/*
  Função que finaliza os sistemas do Allegro e
  libera a memória alocada para a struct Jogo.
*/

void finalizar_jogo(Jogo *J) {
  /* Dados usados internamente pelo Allegro. */
  al_destroy_display(J->disp);
  al_destroy_timer(J->timer);
  al_destroy_event_queue(J->queue);

  /* Dados usados pelo jogo em si. */
  free(J->keys);
  finalizar_entidade(J->player); // Função definida em Player.c.
  finalizar_entidade(J->enemy);
  finalizar_entidade(J->enemy2);
  finalizar_mapa(J->mapas[J->mapa - 1]); // Função definida em Mapa.c.
  free(J->mapas);
  free_text_bitmap(J->title);
  free_text_bitmap(J->subtitle);

  /* Finalização definitiva do Allegro. */
  al_uninstall_system();

  /* Libera a memória da struct Jogo em si. */
  free(J);
}
