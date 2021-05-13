#include <stddef.h>
#include "sendchar.h"

#include "esp_rom_sys.h"
//#include "esp_rom_uart.h"
//#include "esp32s2/rom/ets_sys.h"

sendchar_func_t send_func = NULL;

static void sendchar_override(char c) {
    if (send_func != NULL) {
        send_func(c);
    }
}

void print_set_sendchar(sendchar_func_t send) {
    send_func = send;
    esp_rom_install_channel_putc(1, &sendchar_override);
}
