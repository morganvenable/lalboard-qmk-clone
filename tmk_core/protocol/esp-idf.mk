ESP_IDF_DIR = protocol/esp-idf

SRC += $(ESP_IDF_DIR)/usb_main.c

OPT_DEFS += -DPROTOCOL_ESP_IDF
