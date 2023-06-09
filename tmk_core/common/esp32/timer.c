#include <stdint.h>

#include "timer.h"
#include "esp_timer.h"

static uint64_t reset_point = 0;

void timer_init(void) {
    timer_clear();
}

void timer_clear(void) {
    reset_point = esp_timer_get_time();
}

uint16_t timer_read(void) {
    return (uint16_t)timer_read32();
}

uint32_t timer_read32(void) {
	return (uint32_t)((esp_timer_get_time() - reset_point) / 1000);
}

uint16_t timer_elapsed(uint16_t last) {
    return TIMER_DIFF_16(timer_read(), last);
}

uint32_t timer_elapsed32(uint32_t last) {
    return TIMER_DIFF_32(timer_read32(), last);
}
