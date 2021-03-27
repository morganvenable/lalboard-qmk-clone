#include "driver/gpio.h"

typedef gpio_num_t pin_t;

void setPinInput(pin_t pin);
void setPinInputHigh(pin_t pin);
void setPinInputLow(pin_t pin);
void setPinOutput(pin_t pin);

#define writePinHigh(pin) gpio_set_level(pin, 1)
#define writePinLow(pin) gpio_set_level(pin, 0)
#define writePin(pin, level) gpio_set_level(pin, level)

#define readPin(pin) gpio_get_level(pin)
#define togglePin(pin) writePin(pin, !readPin(pin))
