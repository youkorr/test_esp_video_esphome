#pragma once

typedef struct esp_cam_sensor_xclk_t *esp_cam_sensor_xclk_handle_t;
typedef enum {
    ESP_CAM_SENSOR_XCLK_MODE_GPIO = 0,
    ESP_CAM_SENSOR_XCLK_MODE_LED_PWM,
    ESP_CAM_SENSOR_XCLK_MODE_CLOCK_OUT,
} esp_cam_sensor_xclk_mode_t;

typedef struct {
    int gpio_num;
    unsigned int freq_hz;
    esp_cam_sensor_xclk_mode_t mode;
} esp_cam_sensor_xclk_config_t;

static inline int esp_cam_sensor_new_xclk(const esp_cam_sensor_xclk_config_t *, esp_cam_sensor_xclk_handle_t *) { return 0; }
static inline int esp_cam_sensor_del_xclk(esp_cam_sensor_xclk_handle_t) { return 0; }
static inline int esp_cam_sensor_start_xclk(esp_cam_sensor_xclk_handle_t) { return 0; }
static inline int esp_cam_sensor_stop_xclk(esp_cam_sensor_xclk_handle_t) { return 0; }

