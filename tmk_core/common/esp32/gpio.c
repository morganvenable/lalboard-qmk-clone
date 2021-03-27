#include "driver/gpio.h"
#include "gpio.h"

void setPinInput(pin_t pin) {
    gpio_config_t config;

    config.pin_bit_mask = 1 << pin;
    config.mode = GPIO_MODE_INPUT;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&config);
}

void setPinInputHigh(pin_t pin) {
    gpio_config_t config;

    config.pin_bit_mask = 1 << pin;
    config.mode = GPIO_MODE_INPUT;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_ENABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&config);
}

void setPinInputLow(pin_t pin) {
    gpio_config_t config;

    config.pin_bit_mask = 1 << pin;
    config.mode = GPIO_MODE_INPUT;
    config.pull_down_en = GPIO_PULLDOWN_ENABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&config);
}

void setPinOutput(pin_t pin) {
    gpio_config_t config;

    config.pin_bit_mask = 1 << pin;
    config.mode = GPIO_MODE_INPUT_OUTPUT;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&config);
}
