#include "oled.h"
#include "C:\Users\User\Documents\GitHub\ece295pll\twi.h"
#include <string.h>

static uint8_t oled_buffer[OLED_WIDTH * OLED_HEIGHT / 8];

static void oled_write_command(uint8_t cmd);
static void oled_write_data_block(uint8_t *data, uint16_t len);

static const uint8_t font5x7[][5] = {
    // Space to Z, minimal set
    [' '] = {0x00,0x00,0x00,0x00,0x00},
    [':'] = {0x00,0x36,0x36,0x00,0x00},
    ['.'] = {0x00,0x60,0x60,0x00,0x00},
    ['0'] = {0x3E,0x51,0x49,0x45,0x3E},
    ['1'] = {0x00,0x42,0x7F,0x40,0x00},
    ['2'] = {0x42,0x61,0x51,0x49,0x46},
    ['3'] = {0x21,0x41,0x45,0x4B,0x31},
    ['4'] = {0x18,0x14,0x12,0x7F,0x10},
    ['5'] = {0x27,0x45,0x45,0x45,0x39},
    ['6'] = {0x3C,0x4A,0x49,0x49,0x30},
    ['7'] = {0x01,0x71,0x09,0x05,0x03},
    ['8'] = {0x36,0x49,0x49,0x49,0x36},
    ['9'] = {0x06,0x49,0x49,0x29,0x1E},
    ['A'] = {0x7E,0x11,0x11,0x11,0x7E},
    ['C'] = {0x3E,0x41,0x41,0x41,0x22},
    ['E'] = {0x7F,0x49,0x49,0x49,0x41},
    ['F'] = {0x7F,0x09,0x09,0x09,0x01},
    ['G'] = {0x3E,0x41,0x49,0x49,0x3A},
    ['H'] = {0x7F,0x08,0x08,0x08,0x7F},
    ['L'] = {0x7F,0x40,0x40,0x40,0x40},
    ['M'] = {0x7F,0x02,0x0C,0x02,0x7F},
    ['N'] = {0x7F,0x04,0x08,0x10,0x7F},
    ['O'] = {0x3E,0x41,0x41,0x41,0x3E},
    ['P'] = {0x7F,0x09,0x09,0x09,0x06},
    ['Q'] = {0x3E,0x41,0x51,0x21,0x5E},
    ['R'] = {0x7F,0x09,0x19,0x29,0x46},
    ['T'] = {0x01,0x01,0x7F,0x01,0x01},
    ['X'] = {0x63,0x14,0x08,0x14,0x63}
};

static void oled_write_command(uint8_t cmd) {
    twi_start();
    twi_MT_SLA_W(OLED_ADDR);
    twi_MT_write(0x00);   // control byte: command
    twi_MT_write(cmd);
    twi_stop();
}

static void oled_write_data_block(uint8_t *data, uint16_t len) {
    twi_start();
    twi_MT_SLA_W(OLED_ADDR);
    twi_MT_write(0x40);   // control byte: data
    for (uint16_t i = 0; i < len; i++) {
        twi_MT_write(data[i]);
    }
    twi_stop();
}

void oled_init(void) {
    memset(oled_buffer, 0, sizeof(oled_buffer));

    oled_write_command(0xAE); // display off
    oled_write_command(0xD5); oled_write_command(0x80);
    oled_write_command(0xA8); oled_write_command(0x3F);
    oled_write_command(0xD3); oled_write_command(0x00);
    oled_write_command(0x40);
    oled_write_command(0x8D); oled_write_command(0x14);
    oled_write_command(0x20); oled_write_command(0x00); // horizontal addressing
    oled_write_command(0xA1);
    oled_write_command(0xC8);
    oled_write_command(0xDA); oled_write_command(0x12);
    oled_write_command(0x81); oled_write_command(0x7F);
    oled_write_command(0xD9); oled_write_command(0xF1);
    oled_write_command(0xDB); oled_write_command(0x40);
    oled_write_command(0xA4);
    oled_write_command(0xA6);
    oled_write_command(0xAF); // display on

    oled_clear();
    oled_update();
}

void oled_clear(void) {
    memset(oled_buffer, 0, sizeof(oled_buffer));
}

void oled_draw_pixel(uint8_t x, uint8_t y, bool color) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;

    uint16_t index = x + (y / 8) * OLED_WIDTH;
    if (color)
        oled_buffer[index] |= (1 << (y & 7));
    else
        oled_buffer[index] &= ~(1 << (y & 7));
}

void oled_draw_char(uint8_t x, uint8_t y, char c) {
    if ((uint8_t)c >= 128) c = ' ';

    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = font5x7[(uint8_t)c][col];
        for (uint8_t row = 0; row < 7; row++) {
            oled_draw_pixel(x + col, y + row, (line >> row) & 0x01);
        }
    }
}

void oled_draw_string(uint8_t x, uint8_t y, const char *s) {
    while (*s) {
        oled_draw_char(x, y, *s++);
        x += 6;
        if (x > 122) break;
    }
}

void oled_update(void) {
    oled_write_command(0x21); // column addr
    oled_write_command(0x00);
    oled_write_command(0x7F);

    oled_write_command(0x22); // page addr
    oled_write_command(0x00);
    oled_write_command(0x07);

    oled_write_data_block(oled_buffer, sizeof(oled_buffer));
}

void oled_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color) {
    for (uint8_t i = 0; i < w; i++) {
        for (uint8_t j = 0; j < h; j++) {
            oled_draw_pixel(x + i, y + j, color);
        }
    }
}