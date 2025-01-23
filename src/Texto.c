#include <Texto.h>
#include <Mapa.h>

Text *create_text_bitmap(ALLEGRO_DISPLAY *display, char *string, ALLEGRO_FONT *font, ALLEGRO_COLOR color) {
  int width = strlen(string) * FONT_SIZE;
  int height = FONT_SIZE;
  Text *text = malloc(sizeof(Text));
  text->w = width;
  text->h = height;
  text->font = font;
  text->color = color;
  ALLEGRO_BITMAP *bmp = al_create_bitmap(width, height);
  al_set_target_bitmap(bmp);
  al_draw_text(font, color, 0, 0, 0, string);
  al_set_target_backbuffer(display);
  text->bmp = bmp;
  return text;
}

void draw_centered_scaled_text(Text *text, float scale, int gap_x, int gap_y) {
  float scaled_w = text->w * scale;
  float scaled_h = text->h * scale;
  float center_x = (MAPA_LARGURA_PX - scaled_w) / 2;
  float center_y = (MAPA_ALTURA_PX - scaled_h) / 2;
  al_draw_tinted_scaled_bitmap(text->bmp, al_map_rgb(0, 0, 0), 0, 0, text->w, text->h, center_x + gap_x + 2, center_y + gap_y + 2, scaled_w, scaled_h, 0);
  al_draw_scaled_bitmap(text->bmp, 0, 0, text->w, text->h, center_x + gap_x, center_y + gap_y, scaled_w, scaled_h, 0);
}

Text *edit_text_bitmap(ALLEGRO_DISPLAY *display, Text *text, char *string) {
  Text *new_text = create_text_bitmap(display, string, text->font, text->color);
  free_text_bitmap(text);
  return new_text;
}

void free_text_bitmap(Text *text) {
  al_destroy_bitmap(text->bmp);
  free(text);
}