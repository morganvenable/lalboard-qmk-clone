#include <stdbool.h>
#include <stdint.h>

void suspend_idle(uint8_t timeout) {}

void suspend_power_down(void) {}
bool suspend_wakeup_condition(void) { return true; }
void suspend_wakeup_init(void) {}

void suspend_wakeup_init_user(void) {}
void suspend_wakeup_init_kb(void) {}
void suspend_power_down_user(void) {}
void suspend_power_down_kb(void) {}
