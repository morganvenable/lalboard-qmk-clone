#pragma once

#include <stdio.h>

#define print(s) puts(PSTR(s))
#define println(s) puts(PSTR(s "\r\n"))
#define uprint(s) print(s)
#define uprintln(s) println(s)
#define uprintf(fmt, ...) printf(fmt, ##__VA_ARGS__)

//TODO(jesusfreke): Implement the print redirection functionality
#define xprintf(fmt, ...) printf(fmt, ##__VA_ARGS__)
