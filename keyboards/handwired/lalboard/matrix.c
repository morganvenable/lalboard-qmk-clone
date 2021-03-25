/* Copyright 2017-2019 Nikolaus Wittenstein <nikolaus.wittenstein@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "lalboard.h"

#include "matrix.h"
#include "action.h"

#include <stdint.h>
#include <stdbool.h>

matrix_row_t matrix_get_row(uint8_t row) {
    // TODO: return the requested row data
    return 0;
}

void matrix_print(void) {
    // TODO: use print() to dump the current matrix state to console
}

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

void matrix_init(void) {
    // TODO: initialize hardware and global matrix state here

    // This *must* be called for correct keyboard behavior
    matrix_init_quantum();
}

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_scan_user(void) {}

uint8_t matrix_scan(void) {
    bool matrix_has_changed = false;

    // TODO: add matrix scanning routine here

    // This *must* be called for correct keyboard behavior
    matrix_scan_quantum();

    return matrix_has_changed;
}
