TINYUSB_DIR = protocol/tinyusb
TINYUSB_LIB_DIR = $(LIB_PATH)/tinyusb

SRC += $(TINYUSB_DIR)/main.c
SRC += usb_descriptor.c

TINYUSB_SRC = $(TINYUSB_LIB_DIR)/src/portable/espressif/esp32s2/dcd_esp32s2.c \
    $(TINYUSB_LIB_DIR)/hw/bsp/esp32s2_saola_1/esp32s2_saola_1.c \
    $(TINYUSB_LIB_DIR)/hw/bsp/esp32s2_saola_1/led_strip/src/led_strip_rmt_ws2812.c \
    $(TINYUSB_LIB_DIR)/src/class/cdc/cdc_device.c \
    $(TINYUSB_LIB_DIR)/src/class/hid/hid_device.c \
    $(TINYUSB_LIB_DIR)/src/class/midi/midi_device.c \
    $(TINYUSB_LIB_DIR)/src/class/msc/msc_device.c \
    $(TINYUSB_LIB_DIR)/src/class/vendor/vendor_device.c \
    $(TINYUSB_LIB_DIR)/src/common/tusb_fifo.c \
    $(TINYUSB_LIB_DIR)/src/device/usbd_control.c \
    $(TINYUSB_LIB_DIR)/src/device/usbd.c \
    $(TINYUSB_LIB_DIR)/src/tusb.c

SRC += $(TINYUSB_SRC)

VPATH += $(TINYUSB_LIB_DIR)/src
VPATH += $(TINYUSB_LIB_DIR)/hw
VPATH += $(TINYUSB_LIB_DIR)/hw/bsp/esp32s2_saola_1/led_strip/include

VPATH += $(TMK_PATH)/$(TINYUSB_DIR)

# For descriptor definitions
VPATH += $(TMK_PATH)/protocol/chibios/lufa_utils

OPT_DEFS += -DPROTOCOL_TINYUSB
OPT_DEFS += -DFIXED_CONTROL_ENDPOINT_SIZE=64
OPT_DEFS += -DFIXED_NUM_CONFIGURATIONS=1

OPT_DEFS += -DCFG_TUSB_MCU=OPT_MCU_ESP32S2
OPT_DEFS += -DCFG_TUSB_DEBUG=0

