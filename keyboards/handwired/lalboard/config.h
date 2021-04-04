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

#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
/* VID/PID from esp-idf's default tinyusb configuration */
#define VENDOR_ID       0x303A
#define PRODUCT_ID      0x4004
#define DEVICE_VER      0x0001
#define MANUFACTURER    JesusFreke
#define PRODUCT         lalboard

/* key matrix size */
#define MATRIX_ROWS 10
#define MATRIX_COLS 5

#define MATRIX_ROW_PINS { 0, 1, 2, 3, 4 }
#define MATRIX_COL_PINS { 5, 6, 7, 8, 9 }

// The column with the "down" keys on both the normal and thumb clusters
// is inverted.
#define MATRIX_COL_PUSHED_STATES { 0, 0, 0, 0, 1 }

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 0
