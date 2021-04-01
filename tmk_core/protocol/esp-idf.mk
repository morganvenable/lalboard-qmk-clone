ESP_IDF_DIR = protocol/esp-idf

SRC += $(ESP_IDF_DIR)/main.c
SRC += usb_descriptor.c

VPATH += $(TMK_PATH)/protocol/chibios/lufa_utils

OPT_DEFS += -DPROTOCOL_ESP_IDF
OPT_DEFS += -DFIXED_CONTROL_ENDPOINT_SIZE=64
OPT_DEFS += -DFIXED_NUM_CONFIGURATIONS=1
