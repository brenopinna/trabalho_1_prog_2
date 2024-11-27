#include <allegro5/allegro5.h>
#include <stdbool.h>
#include <stdlib.h>
#include <Jogo.h>
// #include <allegro5/allegro_font.h>

struct Jogo {
  ALLEGRO_DISPLAY *disp;
  ALLEGRO_TIMER *timer;
  ALLEGRO_EVENT_QUEUE *queue;
  // ALLEGRO_FONT *font;
  bool *keys;
  ALLEGRO_EVENT event;
};

Jogo *novo_jogo() {
  al_init();
  al_install_keyboard();

  Jogo *J = malloc(sizeof(Jogo));

  J->disp = al_create_display(600, 400);
  J->timer = al_create_timer(1.0 / 10.0);
  J->queue = al_create_event_queue();
  J->keys = calloc(ALLEGRO_KEY_MAX, sizeof(bool));

  al_register_event_source(J->queue, al_get_keyboard_event_source());
  al_register_event_source(J->queue, al_get_display_event_source(J->disp));
  al_register_event_source(J->queue, al_get_timer_event_source(J->timer));
  al_start_timer(J->timer);

  return J;
}

bool jogo_rodando(Jogo *J) {
  if (J->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
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
  }

  if (redraw && al_is_event_queue_empty(J->queue)) {
    if (J->keys[ALLEGRO_KEY_DOWN] || J->keys[ALLEGRO_KEY_UP]) {
      // fazer alguma coisa
    }
    al_clear_to_color(al_map_rgb(150, 150, 200));
    al_flip_display();
    redraw = false;
  }

}

void finalizar_jogo(Jogo *J) {
  al_destroy_display(J->disp);
  al_destroy_timer(J->timer);
  al_destroy_event_queue(J->queue);
  free(J->keys);
  free(J);
}
