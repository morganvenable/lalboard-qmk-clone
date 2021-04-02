#include <stdint.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "action.h"
#include "bsp/board.h"
#include "class/hid/hid_device.h"
#include "debug.h"
#include "esp_log.h"
#include "keyboard.h"
#include "host.h"
#include "host_driver.h"
#include "report.h"
#include "suspend.h"
#include "tusb.h"
#include "usb_descriptor.h"

#include <LUFA/Drivers/USB/USB.h>

uint8_t keyboard_protocol = 1;
uint8_t keyboard_idle = 0;
static uint8_t keyboard_led_state = 0;

static report_keyboard_t current_keyboard_report;

#ifdef NKRO_ENABLE
#    include "keycode_config.h"
extern keymap_config_t keymap_config;
#endif

// TODO(jesusfreke): add support for GET_REPORT for the other interfaces as well

// static task for usbd
#define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE)
/*#if CFG_TUSB_DEBUG

#else
  #define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE/2)
#endif*/

StackType_t  usb_device_stack[USBD_STACK_SIZE];
StaticTask_t usb_device_taskdef;

void usb_device_task(void* param);

void suspend_task(void);

uint8_t keyboard_leds(void);
void send_keyboard(report_keyboard_t *report);
void send_mouse(report_mouse_t *report);
void send_system(uint16_t data);
void send_consumer(uint16_t data);

host_driver_t esp_idf_driver = {keyboard_leds, send_keyboard, send_mouse, send_system, send_consumer};

int main(void) __attribute__((weak));

// The entry point for an esp-idf project
void app_main(void) {
    printf("in app_main (printf)");
    print("in app_main (print)");

    main();
}

int main(void) {
    board_init();

    host_set_driver(&esp_idf_driver);

    // Create a task for tinyusb device stack
    (void) xTaskCreateStatic( usb_device_task, "usbd", USBD_STACK_SIZE, NULL, configMAX_PRIORITIES-1, usb_device_stack, &usb_device_taskdef);

    print("Keyboard start.\n");
    while (true) {

#if !defined(NO_USB_STARTUP_CHECK)
        suspend_task();
#endif

        keyboard_task();

#ifdef VIRTSER_ENABLE
        virtser_task();
        CDC_Device_USBTask(&cdc_device);
#endif

#ifdef RAW_ENABLE
        raw_hid_task();
#endif

        // Run housekeeping
        housekeeping_task_kb();
        housekeeping_task_user();

        vTaskDelay(1);
    }
}

void suspend_task() {

    // Remote wakeup
    if (tud_suspended()) {
        suspend_power_down();

        if (suspend_wakeup_condition()) {
            if (tud_remote_wakeup()) {
                clear_keyboard();

#    if USB_SUSPEND_WAKEUP_DELAY > 0
                // Some hubs, kvm switches, and monitors do
                // weird things, with USB device state bouncing
                // around wildly on wakeup, yielding race
                // conditions that can corrupt the keyboard state.
                //
                // Pause for a while to let things settle...
                wait_ms(USB_SUSPEND_WAKEUP_DELAY);
#    endif
            }
        }
    }
}

void usb_device_task(void* param) {
    tusb_init();
    while (true) {
        tud_task();
    }
}

uint8_t const * tud_descriptor_device_cb(void) {
    void * descriptor;
    ESP_LOGE("app", "tud_descriptor_device_cb");
    get_usb_descriptor(DTYPE_Device << 8, 0, (const void **const) &descriptor);
    return descriptor;
}

uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
    void *descriptor;
    ESP_LOGE("app", "tud_descriptor_configuration_cb %d", index);
    get_usb_descriptor(DTYPE_Configuration << 8, 0, (const void **const) &descriptor);
    return descriptor;
}

uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf) {
    void *descriptor;
    ESP_LOGE("app", "tud_hid_descriptor_report_cb %d", itf);
    get_usb_descriptor(
        HID_DTYPE_Report << 8,
        tud_hid_itf_index_to_num(itf),
        (const void **const) &descriptor);
    return descriptor;
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    uint8_t itf_num = tud_hid_itf_index_to_num(itf);

    ESP_LOGE("app", "tud_hid_get_report_cb %d %d", itf, report_id);

    if (itf_num == KEYBOARD_INTERFACE) {
        // TODO(jesusfreke): This matches the behavior of LUFA, but I'm not convinced it is totally correct.
        // - This should probably compare the size of current_keyboard_report to reqlen
        // - This should look at the report type and report id, and handle those accordingly
        memcpy(buffer, &current_keyboard_report, reqlen);
        return reqlen;
    }
    return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    uint8_t itf_num = tud_hid_itf_index_to_num(itf);

    switch (itf_num) {
        case KEYBOARD_INTERFACE:
#if defined(SHARED_EP_ENABLE) && !defined(KEYBOARD_SHARED_EP)
        case SHARED_INTERFACE:
#endif

            if (bufsize == 2) {
                uint8_t report_id = buffer[0];

                if (report_id == REPORT_ID_KEYBOARD || report_id == REPORT_ID_NKRO) {
                    keyboard_led_state = buffer[1];
                }
            } else {
                keyboard_led_state = buffer[0];
            }
            break;
    }
}

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    void *descriptor;
    ESP_LOGI("app", "tud_descriptor_string_cb %d", index);
    get_usb_descriptor(DTYPE_String << 8 | index, 0, (const void **const) &descriptor);
    return descriptor;
}

void tud_hid_boot_mode_cb(uint8_t itf, uint8_t boot_mode) {
    uint8_t itf_num = tud_hid_itf_index_to_num(itf);

    if (itf_num == KEYBOARD_INTERFACE) {
        keyboard_protocol = boot_mode ? 0 : 1;
        clear_keyboard();
    }
}

uint8_t keyboard_leds(void) {
    return keyboard_led_state;
}

void send_keyboard(report_keyboard_t *report) {
    uint8_t itf_index = tud_hid_itf_num_to_index(KEYBOARD_INTERFACE);
    size_t report_size = sizeof(report_keyboard_t);

#ifdef NKRO_ENABLE
    if (keyboard_protocol && keymap_config.nkro) {
        itf_index = tud_hid_itf_num_to_index(SHARED_INTERFACE);;
        report_size = sizeof(struct nkro_report);
    }
#endif

    if (!tud_hid_n_ready(itf_index)) return;

    tud_hid_n_report(
        itf_index,
        // Since our report already includes the report id if needed,
        // the report id here should be 0. Otherwise, tud_hid_n_report
        // will add a duplicate report_id field to the beginning
        // of the report
        0,
        report,
        report_size);
}


void send_mouse(report_mouse_t *report) {
#ifdef MOUSE_ENABLE
    uint8_t itf_index = tud_hid_itf_num_to_index(MOUSE_INTERFACE);
    size_t report_size = sizeof(report_mouse_t);

    if (!tud_hid_n_ready(itf_index)) return;

    tud_hid_n_report(
        itf_index,
        // Since our report already includes the report id if needed,
        // the report id here should be 0. Otherwise, tud_hid_n_report
        // will add a duplicate report_id field to the beginning
        // of the report
        0,
        report,
        report_size);
#endif
}

#ifdef EXTRAKEY_ENABLE
static void send_extra(uint8_t report_id, uint16_t data) {
    uint8_t itf_index = tud_hid_itf_num_to_index(SHARED_INTERFACE);

    if (!tud_hid_n_ready(itf_index)) return;

    report_extra_t report = {.report_id = report_id, .usage = data};

    tud_hid_n_report(
        itf_index,
        // Since our report already includes the report id if needed,
        // the report id here should be 0. Otherwise, tud_hid_n_report
        // will add a duplicate report_id field to the beginning
        // of the report
        0,
        &report,
        sizeof(report));
}
#endif

void send_system(uint16_t data) {
#ifdef EXTRAKEY_ENABLE
    send_extra(REPORT_ID_SYSTEM, data);
#endif
}

void send_consumer(uint16_t data) {
#ifdef EXTRAKEY_ENABLE
    send_extra(REPORT_ID_CONSUMER, data);
#endif
}

int tinyusb_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    esp_log_write(ESP_LOG_INFO, "tinyusb", format, args);
    va_end(args);
    return 0;
}
