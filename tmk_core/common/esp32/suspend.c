#include <stdbool.h>
#include <stdint.h>

#include "action_util.h"

#include "host.h"
#include "led.h"
#include "matrix.h"

#ifdef AUDIO_ENABLE
#    include "audio.h"
#endif /* AUDIO_ENABLE */

#ifdef BACKLIGHT_ENABLE
#    include "backlight.h"
#endif

#if defined(RGBLIGHT_SLEEP) && defined(RGBLIGHT_ENABLE)
#    include "rgblight.h"
#endif

#if defined(MOUSEKEY_ENABLE)
#    include "mousekey.h"
#endif

void suspend_idle(uint8_t timeout) {
    // not used
}

__attribute__((weak)) void suspend_power_down_user(void) {}
__attribute__((weak)) void suspend_power_down_kb(void) { suspend_power_down_user(); }

void suspend_power_down(void) {
    suspend_power_down_kb();

#ifndef NO_SUSPEND_POWER_DOWN
    // Turn off backlight
#    ifdef BACKLIGHT_ENABLE
    backlight_set(0);
#    endif

    // Turn off LED indicators
    uint8_t leds_off = 0;
#    if defined(BACKLIGHT_CAPS_LOCK) && defined(BACKLIGHT_ENABLE)
    if (is_backlight_enabled()) {
        // Don't try to turn off Caps Lock indicator as it is backlight and backlight is already off
        leds_off |= (1 << USB_LED_CAPS_LOCK);
    }
#    endif
    led_set(leds_off);

    // Turn off audio
#    ifdef AUDIO_ENABLE
    stop_all_notes();
#    endif

    // Turn off underglow
#    if defined(RGBLIGHT_SLEEP) && defined(RGBLIGHT_ENABLE)
    rgblight_suspend();
#    endif

    // TODO(jesusfreke): power down, and set a wakeup timer..
#endif
}

__attribute__((weak)) void matrix_power_up(void) {}
__attribute__((weak)) void matrix_power_down(void) {}
bool                       suspend_wakeup_condition(void) {
    matrix_power_up();
    matrix_scan();
    matrix_power_down();
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        if (matrix_get_row(r)) return true;
    }
    return false;
}

__attribute__((weak)) void suspend_wakeup_init_user(void) {}
__attribute__((weak)) void suspend_wakeup_init_kb(void) { suspend_wakeup_init_user(); }

void suspend_wakeup_init(void) {
    // clear keyboard state
    // need to do it manually, because we're running from ISR
    //  and clear_keyboard() calls print
    // so only clear the variables in memory
    // the reports will be sent from main.c afterwards
    // or if the PC asks for GET_REPORT
    clear_mods();
    clear_weak_mods();
    clear_keys();
#ifdef MOUSEKEY_ENABLE
    mousekey_clear();
#endif /* MOUSEKEY_ENABLE */
#ifdef EXTRAKEY_ENABLE
    host_system_send(0);
    host_consumer_send(0);
#endif /* EXTRAKEY_ENABLE */
#ifdef BACKLIGHT_ENABLE
    backlight_init();
#endif /* BACKLIGHT_ENABLE */
    led_set(host_keyboard_leds());
#if defined(RGBLIGHT_SLEEP) && defined(RGBLIGHT_ENABLE)
    rgblight_wakeup();
#endif
    suspend_wakeup_init_kb();
}
