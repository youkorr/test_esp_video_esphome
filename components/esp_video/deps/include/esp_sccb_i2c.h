#pragma once
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_sccb_io_t* esp_sccb_io_handle_t;

typedef struct {
    uint8_t dev_addr_length;
    uint16_t device_address;
    uint32_t scl_speed_hz;
    uint32_t addr_bits_width;
    uint32_t val_bits_width;
} sccb_i2c_config_t;

esp_err_t sccb_new_i2c_io_esphome(void *i2c_device, const sccb_i2c_config_t *config, esp_sccb_io_handle_t *io_handle);
esp_err_t esp_sccb_transmit_reg_a16v8(esp_sccb_io_handle_t handle, uint16_t reg_addr, uint8_t reg_val);
esp_err_t esp_sccb_transmit_receive_reg_a16v8(esp_sccb_io_handle_t handle, uint16_t reg_addr, uint8_t *reg_val);

#ifdef __cplusplus
}
#endif
