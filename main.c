/*
 * File:   main2.c
 * Author: MinhT
 *
 * Created on February 13, 2026, 1:49 PM
 */

#include <xc.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "C:\Users\User\Documents\GitHub\ece295pll\Si5351.h"
#include "C:\Users\User\Documents\GitHub\ece295pll\twi.h"
#include "C:\Users\User\Documents\GitHub\ece295pll\oled.h"

// Buttons moved to PORTB
#define BTN_UP   PB3
#define BTN_DN   PB4
#define TXEN_SW  PD6

static inline bool button_pressed(uint8_t pin) {
    return !(PINB & (1 << pin));
}

static void debounce_wait_release_button(uint8_t pin) {
    _delay_ms(20);
    while (button_pressed(pin));
    _delay_ms(20);
}

static inline bool txen_active(void) {
    return !(PIND & (1 << TXEN_SW));
}

static void update_lo_outputs(uint32_t fout) {
    uint32_t denom = 1000000UL;
    uint64_t temp = (uint64_t)800000000ULL * denom;
    uint32_t div_total = temp / fout;

    uint32_t div = div_total / denom;
    uint32_t num = div_total % denom;

    setup_clock(SI5351_PLL_A, SI5351_PORT0, div, num, denom);
    setup_clock(SI5351_PLL_A, SI5351_PORT1, div, num, denom);
    set_phase((word)div);
}

static void display_init_screen(void) {
    oled_clear();
    oled_draw_string(0, 0, "PLL LO GENERATOR");
    oled_draw_string(0, 20, "FREQ:");
    oled_draw_string(0, 36, "TX:");
    oled_update();
}

static void display_update_freq(uint32_t fout) {
    char buf[16];
    uint32_t mhz = fout / 1000000UL;

    snprintf(buf, sizeof(buf), "%luMHz", (unsigned long)mhz);

    oled_fill_rect(40, 20, 80, 8, 0);
    oled_draw_string(40, 20, buf);
    oled_update();
}

static void display_update_tx(bool tx_on) {
    oled_fill_rect(24, 36, 40, 8, 0);
    oled_draw_string(24, 36, tx_on ? "ON" : "OFF");
    oled_update();
}

int main(void) {
    DDRB &= ~((1 << BTN_UP) | (1 << BTN_DN));
    PORTB |= ((1 << BTN_UP) | (1 << BTN_DN));

    DDRD &= ~(1 << TXEN_SW);
    PORTD |= (1 << TXEN_SW);
    
    bool last_tx = txen_active();
    uint32_t fout = 10000000UL;
    
    twi_init();
    si5351_init();
    oled_init();
    display_init_screen();
    display_update_freq(fout);
    display_update_tx(last_tx);

    setup_PLL(SI5351_PLL_A, 32, 0, 1);


    enable_clocks(true);
    update_lo_outputs(fout);

    while (1) {
        bool freq_changed = false;
        bool tx_now = txen_active();

        if (button_pressed(BTN_UP)) {
            fout += 10000000UL;
            debounce_wait_release_button(BTN_UP);
            freq_changed = true;
        }

        if (button_pressed(BTN_DN)) {
            if (fout > 10000000UL) {
                fout -= 10000000UL;
            }
            debounce_wait_release_button(BTN_DN);
            freq_changed = true;
        }

        if (freq_changed) {
            update_lo_outputs(fout);
            display_update_freq(fout);
        }

        if (tx_now != last_tx) {
            display_update_tx(tx_now);
            last_tx = tx_now;
        }

        _delay_ms(10);
    }
}