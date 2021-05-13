#pragma once

#include <stdio.h>
#include "esp_rom_sys.h"

#define print(s) xprintf("%s", PSTR(s))
#define println(s) xprintf("%s\r\n", PSTR(s))
#define uprint(s) print(s)
#define uprintln(s) println(s)
#define uprintf(fmt, ...) xprintf(fmt, ##__VA_ARGS__)

//TODO(jesusfreke): Implement the print redirection functionality
#define xprintf(fmt, ...) esp_rom_printf(fmt, ##__VA_ARGS__)
