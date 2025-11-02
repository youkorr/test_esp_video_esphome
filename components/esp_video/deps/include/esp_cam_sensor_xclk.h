#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdint.h>

/**
 * @brief Type du signal XCLK pour les capteurs caméra
 */
typedef enum {
    ESP_CAM_SENSOR_XCLK_MODE_GPIO = 0,  /*!< Utiliser GPIO direct (timer ou PWM) */
    ESP_CAM_SENSOR_XCLK_MODE_LED_PWM,   /*!< Utiliser périphérique LEDC pour générer XCLK */
    ESP_CAM_SENSOR_XCLK_MODE_CLOCK_OUT, /*!< Utiliser sortie d’horloge dédiée */
} esp_cam_sensor_xclk_mode_t;

/**
 * @brief Configuration pour le générateur de XCLK du capteur caméra
 */
typedef struct {
    int gpio_num;                  /*!< Numéro du GPIO utilisé pour XCLK */
    uint32_t freq_hz;              /*!< Fréquence du signal XCLK */
    esp_cam_sensor_xclk_mode_t mode; /*!< Mode de génération */
} esp_cam_sensor_xclk_config_t;

/**
 * @brief Handle opaque pour le générateur XCLK
 */
typedef struct esp_cam_sensor_xclk_t *esp_cam_sensor_xclk_handle_t;

/**
 * @brief Crée un générateur XCLK
 */
esp_err_t esp_cam_sensor_new_xclk(const esp_cam_sensor_xclk_config_t *config,
                                  esp_cam_sensor_xclk_handle_t *ret_handle);

/**
 * @brief Supprime un générateur XCLK
 */
esp_err_t esp_cam_sensor_del_xclk(esp_cam_sensor_xclk_handle_t handle);

/**
 * @brief Démarre le signal XCLK
 */
esp_err_t esp_cam_sensor_start_xclk(esp_cam_sensor_xclk_handle_t handle);

/**
 * @brief Arrête le signal XCLK
 */
esp_err_t esp_cam_sensor_stop_xclk(esp_cam_sensor_xclk_handle_t handle);

#ifdef __cplusplus
}
#endif
