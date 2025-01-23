#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Mapa.h>
#include <ListaEncadeada.h>
#include <Player.h>
#include <Jogo.h>

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

  /* Dados usados pelo jogo em si. */
  Player *player;  // Ponteiro para uma struct Player.
  MapNode *lista_mapas;
  MapNode *mapa_atual;
  int n_mapa; // Mantém registrado qual mapa está aberto no momento.
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

  /* Aloca dinamicamente uma nova struct Jogo e inicializa-a. */
  Jogo *J = malloc(sizeof(Jogo));

  // Dados usados internamente pelo Allegro.
  J->disp = al_create_display(MAPA_LARGURA_PX, MAPA_ALTURA_PX);
  J->timer = al_create_timer(1.0 / 120.0);
  J->queue = al_create_event_queue();

  // Dados usados pelo jogo em si.
  J->keys = calloc(ALLEGRO_KEY_MAX, sizeof(bool)); // Inicializa o vetor do teclado com zeros.
  J->player = criar_player(); // Função definida em Player.c.
  J->lista_mapas = criar_lista_de_mapas();
  J->mapa_atual = J->lista_mapas;
  J->mapa_atual->map = iniciar_mapa(J->disp, J->mapa_atual->arquivo_mapa); // Carrega, inicialmente, o Mapa 1. Função definida em Mapa.c.
  J->n_mapa = 1; // Registra que o mapa carregado foi o primeiro.
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
      mudar_frame(J->player); // Função definida em Player.c.
      J->frame_count = 0; // Reinicia a contagem de quadros renderizados.
    }

    /* Verifica se o usuário deu algum comando de teclado válido
       e executa a ação correspondente ao comando detectado. */

       /* Movimento do jogador. Função mover_player definida em Player.c. */
    if (J->keys[ALLEGRO_KEY_UP]) { // Para cima.
      mover_player(J->player, PLAYER_DIRECAO_CIMA, J->mapa_atual->map);
    } else if (J->keys[ALLEGRO_KEY_DOWN]) { // Para baixo.
      mover_player(J->player, PLAYER_DIRECAO_BAIXO, J->mapa_atual->map);
    } else if (J->keys[ALLEGRO_KEY_RIGHT]) { // Para a direita.
      mover_player(J->player, PLAYER_DIRECAO_DIREITA, J->mapa_atual->map);
    } else if (J->keys[ALLEGRO_KEY_LEFT]) { // Para a esquerda.
      mover_player(J->player, PLAYER_DIRECAO_ESQUERDA, J->mapa_atual->map);
    }

    /* Troca instantânea de mapa a pedido do usuário. Funções
       finalizar_mapa e iniciar_mapa definidas em Mapa.c. */
    else if (J->keys[ALLEGRO_KEY_1]) { // Mapa 1.
      J->n_mapa = 1;
      troca_mapa = true;
    } else if (J->keys[ALLEGRO_KEY_2]) { // Mapa 2.
      J->n_mapa = 2;
      troca_mapa = true;
    } else if (J->keys[ALLEGRO_KEY_3]) { // Mapa 3.
      J->n_mapa = 3;
      troca_mapa = true;
    } else if (J->keys[ALLEGRO_KEY_4]) { // Mapa 4.
      J->n_mapa = 4;
      troca_mapa = true;
    } else if (J->keys[ALLEGRO_KEY_5]) { // Mapa 5.
      J->n_mapa = 5;
      troca_mapa = true;
    }

    // TODO: Isolar essa parte de trocar mapa em outra função e refatorá-la.

    /* Troca o mapa caso o usuário tenha feito a troca instantânea
       de mapa ou caso o jogador tenha passado dos limites do mapa
       (à direita e à esquerda, respectivamente). */

    /* Realiza a troca instantânea de mapa e reseta
      a posição do jogador para a padrão. */
    if (troca_mapa) {
      finalizar_mapa(J->mapa_atual->map);
      J->mapa_atual = selecionar_mapa(J->n_mapa, J->lista_mapas);
      J->mapa_atual->map = iniciar_mapa(J->disp, J->mapa_atual->arquivo_mapa);

      J->player->x = PLAYER_TAMANHO_SPRITE_REDUZIDA;
      J->player->y = PLAYER_TAMANHO_SPRITE_REDUZIDA;
      J->player->direcao = PLAYER_DIRECAO_BAIXO;
    }

    /* Avança para o próximo mapa. */
    if (J->player->x >= MAPA_LARGURA_PX - PLAYER_TAMANHO_SPRITE_REDUZIDA) {
      J->n_mapa++;
      finalizar_mapa(J->mapa_atual->map);
      J->mapa_atual = J->mapa_atual->next;
      J->mapa_atual->map = iniciar_mapa(J->disp, J->mapa_atual->arquivo_mapa);
      J->player->x = 1; // Teleporta o jogador para o lado esquerdo do mapa.
    }

    /* Volta para o mapa anterior. */
    else if (J->player->x <= 0) {
      J->n_mapa--;
      finalizar_mapa(J->mapa_atual->map);
      J->mapa_atual = J->mapa_atual->prev;
      J->mapa_atual->map = iniciar_mapa(J->disp,  J->mapa_atual->arquivo_mapa);
      J->player->x = MAPA_LARGURA_PX - PLAYER_TAMANHO_SPRITE_REDUZIDA - 1; // Teleporta o jogador para o lado direito do mapa.
    }


    /* Lógica de renderização. */

    // Desenha o cenário.
    al_draw_bitmap(J->mapa_atual->map->background, 0, 0, 0);

    // Desenha o quadro certo do sprite do jogador com a direção, posição e tamanho corretos.
    al_draw_scaled_bitmap(J->player->imagem, J->player->frame * PLAYER_TAMANHO_SPRITE, J->player->direcao * PLAYER_TAMANHO_SPRITE,
                          PLAYER_TAMANHO_SPRITE, PLAYER_TAMANHO_SPRITE, J->player->x, J->player->y, PLAYER_TAMANHO_SPRITE_REDUZIDA, PLAYER_TAMANHO_SPRITE_REDUZIDA, 0);

    // Atualiza a tela com o novo quadro renderizado.
    al_flip_display();
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
  finalizar_player(J->player); // Função definida em Player.c.
  finalizar_mapa(J->mapa_atual->map); // Função definida em Mapa.c.
  remover_mapas(J->lista_mapas);

  /* Finalização definitiva do Allegro. */
  al_uninstall_system();

  /* Libera a memória da struct Jogo em si. */
  free(J);
}
