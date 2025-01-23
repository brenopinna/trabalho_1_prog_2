#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#define FONT_SIZE 8

typedef struct Text {
  int w;
  int h;
  ALLEGRO_FONT *font;
  ALLEGRO_COLOR color;
  ALLEGRO_BITMAP *bmp;
} Text;

Text *create_text_bitmap(ALLEGRO_DISPLAY *display, char *text, ALLEGRO_FONT *font, ALLEGRO_COLOR color);

void draw_centered_scaled_text(Text *text, float scale, int gap_x, int gap_y);

Text *edit_text_bitmap(ALLEGRO_DISPLAY *display, Text *text, char *string);

void free_text_bitmap(Text *text);