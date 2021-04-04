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

#include "lalboard.h"
#include "config.h"

#include "action.h"
#include "gpio.h"
#include "matrix.h"
#include "print.h"

#include <stdint.h>
#include <stdbool.h>

static matrix_row_t current_matrix[MATRIX_ROWS];

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
static const uint8_t col_pushed_states[MATRIX_COLS] = MATRIX_COL_PUSHED_STATES;

matrix_row_t matrix_get_row(uint8_t row) {
    return current_matrix[row];
}

void matrix_print(void) {
  print("\nr/c ");
  for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      print_hex8(col);
  }
  print("\n");

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    print_hex8(row);
    print(": ");

    // esp-idf's printf doesn't seem to support the %b specifier, so we'll just
    // print out the bits manually
    matrix_row_t row_value = current_matrix[row];
    for (uint8_t col_bit = 1 << (MATRIX_COLS - 1); col_bit != 0; col_bit >>= 1) {
        print((row_value | col_bit) ? "1" : "0");
    }

    print("\n");
  }
}

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

void matrix_init(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        setPinOutput(row_pins[row]);
        writePin(row_pins[row], 0);
    }

    for (int col = 0; col < MATRIX_COLS; col++) {
        setPinInput(col_pins[col]);
    }

    matrix_init_quantum();
}

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_scan_user(void) {}


matrix_row_t read_row(void) {
    matrix_row_t row = 0;

    for (int col = 0; col < MATRIX_COLS; col++) {
        int col_pin = col_pins[col];

        if (readPin(col_pin) == col_pushed_states[col]) {
            row |= 1 << col;
        }
    }

    return row;
}

uint8_t matrix_scan(void) {
    bool changed = false;

    for (int row = 0; row < MATRIX_ROWS; row++) {
        int row_pin = row_pins[row];
        writePin(row_pin, 1);
        wait_us(15);

        matrix_row_t new_row = read_row();
        changed |= new_row != current_matrix[row];
        current_matrix[row] = new_row;
    }

    matrix_scan_quantum();
    return changed;
}

void matrix_power_down(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        int row_pin = row_pins[row];
        writePin(row_pin, 0);
        wait_us(15);
    }
}
