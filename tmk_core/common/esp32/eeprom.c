#include <stdint.h>
#include <stdlib.h>

#include "debug.h"
#include "eeprom.h"

#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"

#ifndef EEPROM_SIZE
#    include "eeconfig.h"
#    define EEPROM_SIZE (((EECONFIG_SIZE + 3) / 4) * 4)  // based off eeconfig's current usage, aligned to 4-byte sizes, to deal with LTO
#endif

static wl_handle_t partition_handle = WL_INVALID_HANDLE;
static FILE *eeprom_file = NULL;

// Mount path for the emulated eeprom partition
static const char *mount_path = "/eeprom";

// Full path to the backing file for the emulated eeprom
static const char *eeprom_path = "/eeprom/eeprom";

void eeprom_driver_init(void) {
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };

    esp_err_t err = esp_vfs_fat_spiflash_mount(mount_path, "eeprom", &mount_config, &partition_handle);
    if (err != ESP_OK)
    {
        dprintf("eeprom_driver_init: failed to mount fat partition (%s)\n", esp_err_to_name(err));
        partition_handle = WL_INVALID_HANDLE;
        return;
    }

    eeprom_file = fopen(eeprom_path, "r+b");
    if (eeprom_file == NULL) {
        eeprom_file = fopen(eeprom_path, "w+b");
        if (eeprom_file == NULL) {
            dprint("eeprom_driver_init: failed to open file.\n");
            return;
        }
    }
}

void eeprom_driver_erase(void) {
    if (fclose(eeprom_file) != 0) {
        dprint("eeprom_driver_erase: file couldn't be closed.");
    }

    eeprom_file = fopen(eeprom_path, "w+b");
    if (eeprom_path == NULL) {
        dprint("eeprom_driver_init: failed to open file.\n");
        return;
    }
}

void eeprom_read_block(void *buf, const void *addr, size_t len) {
    uint32_t offset = (uint32_t)addr;
    if (fseek(eeprom_file, offset, SEEK_SET) != 0) {
        dprintf("eeprom_read_block: Failed to seek (%d).\n", ferror(eeprom_file));
        clearerr(eeprom_file);
        memset(buf, 0, len);
        return;
    }

    size_t read = fread(buf, 1, len, eeprom_file);
    if (read < len) {
        memset(buf + read, 0, len - read);
    }
    if (ferror(eeprom_file)) {
        dprintf("eeprom_read_block: Error occured while reading (%d).\n", ferror(eeprom_file));
        clearerr(eeprom_file);
    }
}

void eeprom_write_block(const void *buf, void *addr, size_t len) {
    uint32_t offset = (uint32_t)addr;
    if (fseek(eeprom_file, offset, SEEK_SET) != 0) {
        dprintf("eeprom_write_block: Failed to seek (%d).\n", ferror(eeprom_file));
        clearerr(eeprom_file);
        return;
    }

    size_t written = fwrite(buf, 1, len, eeprom_file);
    if (written != len) {
        dprintf("eeprom_write_block: Failed to write entire block (%d).\n", ferror(eeprom_file));
        dprintf("%d requested, %d written.\n", len, written);
        clearerr(eeprom_file);
        return;
    }

    if (fflush(eeprom_file) != 0) {
        dprintf("eeprom_write_block: Failed to flush stream (%d).\n", ferror(eeprom_file));
        clearerr(eeprom_file);
        return;
    }
}
