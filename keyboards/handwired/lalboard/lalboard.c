/* Copyright 2021 Ben Gruver <jesusfreke@jesusfreke.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lalboard.h"
#include "config.h"

#include "driver/uart.h"
#include "esp_err.h"
#include "tusb.h"


// The primary side sends this to the secondary side on boot, once it
// detects that it is the primary (connected to usb)
static uint8_t handshake_magic[4] = { 0x16, 0xcb, 0xf0, 0x30 };

typedef enum {
    LEFT,
    RIGHT
} hand_t;

typedef enum {
    PRIMARY,
    SECONDARY
} primary_t;

//static hand_t hand_status;
static primary_t primary_status;

void keyboard_post_init_kb(void) {

    //debug_enable = true;
    //debug_matrix = true;

    uart_config_t uart_config = {
        .baud_rate = 1000000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    esp_err_t err = uart_driver_install(
        SPLIT_TRANSPORT_UART_NUM,
        SPLIT_TRANSPORT_UART_RX_BUFFER_SIZE,
        SPLIT_TRANSPORT_UART_TX_BUFFER_SIZE,
        0, NULL, 0);
    if (err != ESP_OK) {
        printf("Error while installing serial driver: %s\n", esp_err_to_name(err));
        return;
    }

    err = uart_param_config(SPLIT_TRANSPORT_UART_NUM, &uart_config);
    if (err != ESP_OK) {
        printf("Error while configuring serial params: %s\n", esp_err_to_name(err));
        return;
    }

    // We invert the RX/TX pins on the left side
    err = uart_set_pin(
        SPLIT_TRANSPORT_UART_NUM,
        is_keyboard_left() ? SPLIT_TRANSPORT_UART_RX : SPLIT_TRANSPORT_UART_TX,
        is_keyboard_left() ? SPLIT_TRANSPORT_UART_TX : SPLIT_TRANSPORT_UART_RX,
        UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        printf("Error while configuring serial pins: %s\n", esp_err_to_name(err));
        return;
    }

    dprintf("Performing handshake.\n");

    // Perform an initial handshake, to figure out who is primary and secondary
    while (true) {
        if (tud_connected()) {
            primary_status = PRIMARY;
            size_t bytes = uart_write_bytes(SPLIT_TRANSPORT_UART_NUM, &handshake_magic, sizeof(handshake_magic));
            if (bytes != sizeof(handshake_magic)) {
                printf("Couldn't write entire handshake. Only wrote %d bytes.\n", bytes);
            }
            break;
        }

        size_t bytes_available;
        err = uart_get_buffered_data_len(SPLIT_TRANSPORT_UART_NUM, &bytes_available);
        if (err != ESP_OK) {
            printf("Got error while checking for buffered uart data: %s\n", esp_err_to_name(err));
            continue;
        }
        if (bytes_available >= 4) {
            uint8_t buf[sizeof(handshake_magic)];
            size_t bytes = uart_read_bytes(SPLIT_TRANSPORT_UART_NUM, &buf, sizeof(buf), 0);
            if (bytes != sizeof(buf)) {
                printf("Unexpectedly read fewer than 4 bytes (%d) for the initial handshake.\n", bytes);
            }
            if (memcmp(buf, handshake_magic, sizeof(buf)) != 0) {
                printf("Got unexpected handshake value: %02x %02x %02x %02x.\n", buf[0], buf[1], buf[2], buf[3]);
            }
            primary_status = SECONDARY;
            break;
        }

        vTaskDelay(1);
    }

    dprintf("Handshake completed. Primary: %d", primary_status == PRIMARY);
}

bool is_keyboard_master(void) {
    return primary_status == PRIMARY;
}

bool is_keyboard_left(void) {
#ifdef MASTER_LEFT
    return true;
#else
    return false;
#endif
}
