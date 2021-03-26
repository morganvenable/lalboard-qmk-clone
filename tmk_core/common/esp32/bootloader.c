#include "bootloader.h"
#include "esp_system.h"

void bootloader_jump(void) {
    esp_restart();
}
