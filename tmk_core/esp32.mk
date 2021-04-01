SYSTEM_TYPE := $(shell gcc -dumpmachine)

ifeq ("${MCU_SERIES}", "esp32s2")
	PREFIX = xtensa-esp32s2-elf-
else
	$(error Unknown ESP32 type ${MCU_SERIES})
endif

CC = ${PREFIX}gcc
OBJCOPY = ${PREFIX}objcopy
OBJDUMP = ${PREFIX}objdump
SIZE = ${PREFIX}size
AR = ${PREFIX}ar
NM = ${PREFIX}nm
HEX =
EEP =
BIN =

COMPILEFLAGS += -fomit-frame-pointer
COMPILEFLAGS += -ffunction-sections
COMPILEFLAGS += -fdata-sections
COMPILEFLAGS += -fshort-wchar
COMPILEFLAGS += -mlongcalls
COMPILEFLAGS += -mfix-esp32-psram-cache-issue

CFLAGS += $(COMPILEFLAGS)
CFLAGS += -fno-inline-small-functions
CFLAGS += -fno-strict-aliasing

CXXFLAGS += $(COMPILEFLAGS)
CXXFLAGS += -fno-rtti

OPT_DEFS += -DPLATFORM_ESP_IDF
