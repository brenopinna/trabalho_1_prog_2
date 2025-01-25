#define ALLEGRO_STATICLINK

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <Mapa.h>
#include <ListaEncadeada.h>
#include <Entity.h>
#include <Player.h>
#include <Goal.h>
#include <Texto.h>
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
  ALLEGRO_FONT *font; // Fonte.

  /* Dados usados pelo jogo em si. */

  /* Textos que serão mostrados na tela. */
  Text *title;
  Text *subtitle;

  /* Entidades. */
  Entity *player; // Jogador.
  Entity *enemy; // Inimigo 1.
  Entity *enemy2; // Inimigo 2.
  Entity *goal; // Baú.
  MapNode *lista_mapas; // Ponteiro para o primeiro nó da lista encadeada de mapas.
  MapNode *mapa_atual; // Ponteiro para o mapa que está aberto no momento.
  int n_mapa; // Registra o número do mapa que está aberto no momento.
  bool *keys; // Vetor que armazena o estado das teclas do teclado.
  int frame_count; // Contagem de quadros renderizados. Usada para controlar a velocidade da animação do sprite do jogador.
  bool derrota; // Registra que o usuário foi derrotado.
  bool vitoria; // Registra que o usuário venceu o jogo.
  bool show_title; // Indica se o texto deve ser mostrado na tela ou não.
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

  /* Inicialização dos sistemas do Allegro necessários. */
  al_init();
  al_init_image_addon();
  al_install_keyboard();
  al_init_font_addon();

  /* Aloca dinamicamente uma nova struct Jogo e inicializa-a. */

  Jogo *J = malloc(sizeof(Jogo));

  /* Dados usados internamente pelo Allegro. */
  J->disp = al_create_display(MAPA_LARGURA_PX, MAPA_ALTURA_PX);
  J->timer = al_create_timer(1.0 / 120.0);
  J->queue = al_create_event_queue();
  J->font = al_create_builtin_font();

  /* Dados usados pelo jogo em si. */

  /* Cria os textos da tela de abertura do jogo. */
  J->title = create_text_bitmap(J->disp, "AS AVENTURAS DE JEMENELSON!", J->font, al_map_rgb(255, 255, 255)); // Função definida em Texto.c.
  J->subtitle = create_text_bitmap(J->disp, "pressione qualquer tecla para iniciar.", J->font, al_map_rgb(255, 255, 255));

  J->keys = calloc(ALLEGRO_KEY_MAX, sizeof(bool)); // Inicializa o vetor do teclado com zeros.

  /* Cria as entidades. */
  J->player = criar_player(); // Função definida em Player.c.
  J->enemy = criar_entidade(); // Função definida em Entity.c.
  J->enemy2 = criar_entidade();
  J->goal = criar_objetivo(); // Função definida em Goal.c.

  /* Coloca os inimigos em suas posições iniciais. */
  J->enemy2->x = J->enemy->x = ENTITY_POSICAO_INICIAL_X;
  J->enemy2->y = J->enemy->y = ENTITY_POSICAO_INICIAL_Y;
  
  /* Configuração inicial dos mapas. */
  J->lista_mapas = criar_lista_de_mapas(); // Função definida em ListaEncadeada.c.
  J->mapa_atual = J->lista_mapas; // Define o mapa atual como o primeiro da lista.
  J->mapa_atual->map = iniciar_mapa(J->disp, J->mapa_atual->arquivo_mapa); // Carrega o primeiro mapa da lista. Função definida em Mapa.c.
  J->n_mapa = 1; // Registra que o mapa carregado foi o primeiro.

  J->frame_count = 0; // Inicia a contagem de quadros em 0.
  J->derrota = J->vitoria = false;
  J->show_title = true; // Indica que a tela inicial deve ser mostrada logo no início do jogo.

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
    parar_entidade(J->player); // Se qualquer tecla for solta, o jogador para. Função definida em Entity.c.
  }

  /* Lógica principal da atualização do jogo. Só é executada caso o usuário não tenha vencido
     ou perdido o jogo e não haja nenhum texto aparecendo na tela, sempre em sincronia com o
     temporizador e quando não há mais outros eventos a serem processados. */
  if (J->event.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(J->queue)) {
    if (J->derrota || J->vitoria) return;
    bool troca_mapa = false; // Indica se o mapa foi trocado a pedido do usuário ou não.
    if (!J->show_title) {
      if (!(J->frame_count % 10)) {
        /* Muda a direção dos inimigos para uma direção aleatória, em intervalos
           aleatórios, múltiplos de 10, de pelo menos 70 quadros. */
        if (!(J->frame_count % (70 + (rand() % 4) * 10))) {
          J->enemy->direcao = rand() % 4;
        }
        if (!(J->frame_count % (70 + (rand() % 4) * 10))) {
          J->enemy2->direcao = rand() % 4;
        }

        /* Atualiza os sprites das entidades a cada 10 quadros renderizados.*/
        if (J->enemy->andando)
          mudar_frame(J->enemy); // Função definida em Entity.c.
        if (J->enemy2->andando)
          mudar_frame(J->enemy2);
        if (J->player->andando) {
          mudar_frame(J->player);
        }
      }

      mover_entidade(J->enemy, J->enemy->direcao, J->mapa_atual->map); // Função definida em Entity.c.
      mover_entidade(J->enemy2, J->enemy2->direcao, J->mapa_atual->map);

      /* Verifica se o usuário deu algum comando de teclado válido
         e executa a ação correspondente ao comando detectado. */

      /* Movimento do jogador. Função mover_player definida em Player.c. */
      if (J->keys[ALLEGRO_KEY_UP]) { // Para cima.
        mover_entidade(J->player, ENTITY_DIRECAO_CIMA, J->mapa_atual->map);
      } else if (J->keys[ALLEGRO_KEY_DOWN]) { // Para baixo.
        mover_entidade(J->player, ENTITY_DIRECAO_BAIXO, J->mapa_atual->map);
      } else if (J->keys[ALLEGRO_KEY_RIGHT]) { // Para a direita.
        mover_entidade(J->player, ENTITY_DIRECAO_DIREITA, J->mapa_atual->map);
      } else if (J->keys[ALLEGRO_KEY_LEFT]) { // Para a esquerda.
        mover_entidade(J->player, ENTITY_DIRECAO_ESQUERDA, J->mapa_atual->map);
      }

      /* Troca instantânea de mapa a pedido do usuário. */
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

      /* Faz o usuário ser derrotado se o jogador colidir com algum inimigo. */
      if (colidiu(J->player, J->enemy) || colidiu(J->player, J->enemy2)) {
        J->derrota = true;
      }

      /* Faz o usuário vencer o jogo se o jogador colidir com o baú e estiver no último mapa. */
      if (!J->mapa_atual->next && colidiu(J->player, J->goal)) {
        J->vitoria = true;
      }

      // TODO: Isolar essa parte de trocar mapa em outra função e refatorá-la.

      /* Realiza a troca instantânea de mapa e teleporta o jogador
         e os inimigos para suas posições iniciais. */
      if (troca_mapa) {
        finalizar_mapa(J->mapa_atual->map); // Função definida em Mapa.c.
        J->mapa_atual = selecionar_mapa(J->n_mapa, J->lista_mapas); // Função definida em ListaEncadeada.c.
        J->mapa_atual->map = iniciar_mapa(J->disp, J->mapa_atual->arquivo_mapa);

        J->player->x = ENTITY_TAMANHO_SPRITE_REDUZIDA;
        J->player->y = ENTITY_TAMANHO_SPRITE_REDUZIDA;
        J->player->direcao = ENTITY_DIRECAO_BAIXO;
        J->enemy2->x = J->enemy->x = ENTITY_POSICAO_INICIAL_X;
        J->enemy2->y = J->enemy->y = ENTITY_POSICAO_INICIAL_Y;
      }

      /* Avança para o próximo mapa caso o jogador ultrapasse os limites
         do mapa pela direita. Os inimigos são teleportados para suas
         posições iniciais e o jogador, para o lado esquerdo da tela. */
      if (J->player->x >= MAPA_LARGURA_PX - ENTITY_TAMANHO_SPRITE_REDUZIDA) {
        J->n_mapa++;
        finalizar_mapa(J->mapa_atual->map);
        J->mapa_atual = J->mapa_atual->next;
        J->mapa_atual->map = iniciar_mapa(J->disp, J->mapa_atual->arquivo_mapa);

        J->player->x = 1;
        J->enemy2->x = J->enemy->x = ENTITY_POSICAO_INICIAL_X;
        J->enemy2->y = J->enemy->y = ENTITY_POSICAO_INICIAL_Y;
      }

      /* Volta para o mapa anterior caso o jogador ultrapasse os limites
         do mapa pela esquerda. Os inimigos são teleportados para suas
         posições iniciais e o jogador, para o lado direito da tela. */
      else if (J->player->x <= 0) {
        J->n_mapa--;
        finalizar_mapa(J->mapa_atual->map);
        J->mapa_atual = J->mapa_atual->prev;
        J->mapa_atual->map = iniciar_mapa(J->disp, J->mapa_atual->arquivo_mapa);

        J->player->x = MAPA_LARGURA_PX - ENTITY_TAMANHO_SPRITE_REDUZIDA - 1;
        J->enemy2->x = J->enemy->x = ENTITY_POSICAO_INICIAL_X;
        J->enemy2->y = J->enemy->y = ENTITY_POSICAO_INICIAL_Y;
      }
    }

    /* Lógica de renderização. */

    /* Cria um vetor que armazena a lista de todas as entidades, e coloca-os
       em ordem de "profundidade" para serem renderizados na ordem certa. */
    Entity *entities[] = { J->player, J->enemy, J->enemy2, J->goal };
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

    /* Se o usuário foi derrotado, desenha a tela de derrota. */
    if (J->derrota) {
      J->title = edit_text_bitmap(J->disp, J->title, "VOCÊ PERDEU!");
      J->subtitle = edit_text_bitmap(J->disp, J->subtitle, "Reinicie o jogo e tente novamente.");
      J->show_title = true;
    
    /* Se o usuário venceu o jogo, desenha a tela de vitória. */
    } else if (J->vitoria) {
      J->title = edit_text_bitmap(J->disp, J->title, "VOCÊ VENCEU!");
      J->subtitle = edit_text_bitmap(J->disp, J->subtitle, "Reinicie o jogo para ganhar novamente!");
      J->show_title = true;
    
    /* Caso contrário, renderiza o jogo normal. */
    } else {
      /* Desenha o cenário. */
      al_draw_bitmap(J->mapa_atual->map->background, 0, 0, 0);

      /* Desenha cada uma das entidades, seguindo a ordem do vetor criado anteriormente. */
      for (int i = 0; i < size; i++) {
        if (entities[i]->is_player) { // Jogador.
          al_draw_scaled_bitmap(entities[i]->imagem, entities[i]->frame * ENTITY_TAMANHO_SPRITE, entities[i]->direcao * ENTITY_TAMANHO_SPRITE,
                                ENTITY_TAMANHO_SPRITE, ENTITY_TAMANHO_SPRITE, entities[i]->x, entities[i]->y, ENTITY_TAMANHO_SPRITE_REDUZIDA, ENTITY_TAMANHO_SPRITE_REDUZIDA, 0);
        } else if (entities[i]->is_goal && !J->mapa_atual->next) // Baú (só é desenhado se o usuário estiver no último mapa).
          al_draw_scaled_bitmap(entities[i]->imagem, 0, 0, GOAL_TAMANHO_SPRITE, GOAL_TAMANHO_SPRITE,
                                entities[i]->x, entities[i]->y, GOAL_TAMANHO_SPRITE_REDUZIDA, GOAL_TAMANHO_SPRITE_REDUZIDA, 0);
        else { // Inimigos (são desenhados com um efeito especial de coloração e transparência).
          al_draw_tinted_scaled_bitmap(entities[i]->imagem, al_map_rgba(50, 100, 255, 150), entities[i]->frame * ENTITY_TAMANHO_SPRITE, entities[i]->direcao * ENTITY_TAMANHO_SPRITE,
                                       ENTITY_TAMANHO_SPRITE, ENTITY_TAMANHO_SPRITE, entities[i]->x, entities[i]->y, ENTITY_TAMANHO_SPRITE_REDUZIDA, ENTITY_TAMANHO_SPRITE_REDUZIDA, 0);
        }
      }
    }

    /* Desenha texto na tela. */
    if (J->show_title) {
      if (J->derrota) { // Se o usuário foi derrotado, pinta-se o fundo de vermelho.
        al_clear_to_color(al_map_rgb(200, 0, 0));
      } else if (J->vitoria) { // Se o usuário foi venceu o jogo, pinta-se o fundo de verde.
        al_clear_to_color(al_map_rgb(0, 200, 0));
      }
      draw_centered_scaled_text(J->title, 3, 0, 0);
      draw_centered_scaled_text(J->subtitle, 1.6, 0, FONT_SIZE * 3);
    }
    /* Atualiza a tela com o novo quadro renderizado. */
    al_flip_display();

    /* Aumenta a contagem de quadros renderizados. */
    J->frame_count++;
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
  finalizar_entidade(J->player); // Função definida em Entity.c.
  finalizar_entidade(J->enemy);
  finalizar_entidade(J->enemy2);
  finalizar_entidade(J->goal);
  finalizar_mapa(J->mapa_atual->map); // Função definida em Mapa.c.
  remover_mapas(J->lista_mapas); // Função definida em ListaEncadeada.c.
  free_text_bitmap(J->title); // Função definida em Texto.c.
  free_text_bitmap(J->subtitle);

  /* Finalização definitiva do Allegro. */
  al_uninstall_system();

  /* Libera a memória da struct Jogo em si. */
  free(J);
}
