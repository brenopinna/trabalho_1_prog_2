#include <stdbool.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <Jogo.h>
#include <Player.h>
#include <Mapa.h>

struct Jogo {
  ALLEGRO_DISPLAY *disp;
  ALLEGRO_TIMER *timer;
  ALLEGRO_EVENT_QUEUE *queue;
  ALLEGRO_EVENT event;
  Player *player;
  Map *mapa;
  bool *keys;
  int frame_count;
};

Jogo *novo_jogo() {
  al_init();
  al_init_image_addon();
  al_install_keyboard();

  Jogo *J = malloc(sizeof(Jogo));

  J->disp = al_create_display(MAP_PX_WIDTH, MAP_PX_HEIGHT);
  J->timer = al_create_timer(1.0 / 100.0);
  J->queue = al_create_event_queue();
  J->keys = calloc(ALLEGRO_KEY_MAX, sizeof(bool));
  J->player = criar_player();
  J->mapa = init_map(J->disp);
  J->frame_count = 0;

  al_register_event_source(J->queue, al_get_keyboard_event_source());
  al_register_event_source(J->queue, al_get_display_event_source(J->disp));
  al_register_event_source(J->queue, al_get_timer_event_source(J->timer));
  al_start_timer(J->timer);

  return J;
}

bool jogo_rodando(Jogo *J) {
  if (J->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ||
      (J->event.type == ALLEGRO_EVENT_KEY_DOWN && J->keys[ALLEGRO_KEY_ESCAPE]))
    return false;

  return true;
}

void atualizar_jogo(Jogo *J) {
  bool redraw = true;

  al_wait_for_event(J->queue, &J->event);

  ALLEGRO_EVENT event = J->event;

  if (event.type == ALLEGRO_EVENT_TIMER) {
    redraw = true;
  } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
    J->keys[event.keyboard.keycode] = true;
  } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
    J->keys[event.keyboard.keycode] = false;
    parar_player(J->player);
  }

  Player *player = J->player;
  const bool *keys = J->keys;

  if (redraw && al_is_event_queue_empty(J->queue)) {
    J->frame_count++;

    if (J->frame_count >= 10 && player->andando) {
      muda_frame(J->player);
      J->frame_count = 0;
    }

    if (keys[ALLEGRO_KEY_UP]) {
      move_player(J->player, PLAYER_DIRECTION_UP);
    } else if (keys[ALLEGRO_KEY_DOWN]) {
      move_player(J->player, PLAYER_DIRECTION_DOWN);
    } else if (keys[ALLEGRO_KEY_RIGHT]) {
      move_player(J->player, PLAYER_DIRECTION_RIGHT);
    } else if (keys[ALLEGRO_KEY_LEFT]) {
      move_player(J->player, PLAYER_DIRECTION_LEFT);
    }

    al_draw_bitmap(J->mapa->background, 0, 0, 0);
    al_draw_scaled_bitmap(player->image, player->frame * PLAYER_SPRITE_SIZE, player->direction * PLAYER_SPRITE_SIZE, PLAYER_SPRITE_SIZE, PLAYER_SPRITE_SIZE, player->x, player->y, PLAYER_SCALED_SPRITE_SIZE, PLAYER_SCALED_SPRITE_SIZE, 0);

    al_flip_display();

    redraw = false;
  }
}

void finalizar_jogo(Jogo *J) {
  al_destroy_display(J->disp);
  al_destroy_timer(J->timer);
  al_destroy_event_queue(J->queue);
  free(J->keys);
  finalizar_player(J->player);
  finalizar_mapa(J->mapa);
  al_uninstall_system();
  free(J);
}
