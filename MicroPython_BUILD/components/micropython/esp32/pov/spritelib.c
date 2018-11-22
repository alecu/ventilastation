#include <esp_system.h>
#include "spritelib.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define COLUMNS 256
#define PIXELS 52
#define ROWS 128

const byte TRANSPARENT = 0xFF;
byte deepspace[] = {51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 35, 34, 33, 32, 31, 30, 30, 29, 28, 27, 27, 26, 25, 25, 24, 23, 23, 22, 21, 21, 20, 19, 19, 18, 18, 17, 17, 16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 11, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


extern const uint32_t palette_pal[];
extern const uint8_t galaga_8_png[];
extern const uint8_t explosion_png[];
extern const uint8_t galaga_10_png[];
extern const uint8_t galaga_png[];
extern const uint8_t disparo_png[];
extern uint32_t* pixels;

Sprite sprites[NUM_SPRITES];
Sprite* nave = &sprites[0];
Sprite* disparo = &sprites[1];

#define STARS COLUMNS/2

typedef struct {
  byte x;
  byte y;
} Star;
Star starfield[STARS];

int random(int max) {
    return esp_random() % max;
}

void init_sprites() {

  for (int f = 0; f<STARS; f++) {
    starfield[f].x = random(COLUMNS);
    starfield[f].y = random(PIXELS);
  }

  int x = 16;

  for (int i = 0; i<3; i++) {
    int w;
    const byte* im;
    switch(i) {
      default:
      case 0:
        w = 16;
        im = galaga_png;
        break;
      case 1:
        w = 10;
        im = galaga_10_png;
        break;
      case 2:
        w = 8;
        im = galaga_8_png;
        break;
    }
    for (int n = 0; n<6; n++) {
        Sprite* s = &sprites[10 + n + 6*i];
  
        s->enabled = false;
        s->w = w;
        s->h = w;
        s->x = x; x+=w+1;
        s->y = PIXELS-s->h + n;
        s->frame = 0;
        s->image = im;
        im += w * w * 2;
    }
  }


  disparo->enabled = false;
  disparo->w = 3;
  disparo->h = 5;
  disparo->x = 48;
  disparo->y = 12;
  disparo->image = disparo_png;

  nave->enabled = true;
  nave->w = 16;
  nave->h = 16;
  nave->x = 256-8;
  nave->y = 0;
  nave->image = galaga_png; // w*h*skip_imgs*frames

}

void step() {
  for (int n = 0; n<40; n++) {
      Sprite* s = &sprites[n];
      s->frame = (s->frame+1) % 2;
  }
}

void step_starfield() {
  for (int f=0; f<STARS; f++) {
    if(starfield[f].y++ > PIXELS) {
      starfield[f].y = 0;
      starfield[f].x = random(COLUMNS);
    }
  }
}

int inline get_visible_column(int sprite_x, int sprite_width, int render_column) {
    int sprite_column = (render_column - sprite_x + COLUMNS) % COLUMNS;
    if (0 <= sprite_column && sprite_column < sprite_width) {
        return sprite_column;
    } else {
        return -1;
    }
}

void render(int x) {
  x = x % COLUMNS;
  for (int y=0; y<PIXELS; y++) {
    pixels[y] = 0x000000ff;
  }
  for (int f=0; f<STARS; f++) {
    if (starfield[f].x == x) {
      pixels[starfield[f].y] = 0x040404ff;
    }
  }
  /*
  int base = (x % 8) * PIXELS;
  for (int y=0; y<PIXELS; y++) {
    byte color = background[base + y];
    pixels[PIXELS - 1 - y] = palette_pal[color];
  }
  */

  // el sprite 0 se dibuja arriba de todos los otros
  for (int n=NUM_SPRITES-1; n>=0; n--) {
    Sprite* s = &sprites[n%NUM_SPRITES];
    if (!s->enabled) {
      continue;
    }
    int visible_column = get_visible_column(s->x, s->w, x);
    if (visible_column != -1) {
      int desde = MAX(s->y, 0);
      int hasta = MIN(s->y + s->h, ROWS-1);
      int comienzo = MAX(-s->y, 0);
      int base = visible_column * s->h + (s->frame * s->w * s->h);

      byte* imagen = s->image + base + comienzo;
      for(int y=desde; y<hasta; y++, imagen++) {
        byte color = *imagen;
        if (color != TRANSPARENT) {
          pixels[deepspace[y]] = palette_pal[color];
        }
      }
    }
  }
}