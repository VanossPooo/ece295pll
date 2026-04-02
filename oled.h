#ifndef OLED_H
#define OLED_H

#include <stdint.h>
#include <stdbool.h>

#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

void oled_init(void);
void oled_clear(void);
void oled_update(void);

void oled_draw_pixel(uint8_t x, uint8_t y, bool color);
void oled_draw_char(uint8_t x, uint8_t y, char c);
void oled_draw_string(uint8_t x, uint8_t y, const char *s);
void oled_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color);

#endif