#include "esp_video_wrapper.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32_VARIANT_ESP32P4

namespace esphome {
namespace esp_video_wrapper {

static const char *TAG = "esp_video_wrapper";

void ESPVideoWrapper::setup() {
  ESP_LOGI(TAG, "🎬 Initialisation ESP-VIDEO Wrapper");
}

bool ESPVideoWrapper::initialize_video_system(
    uint8_t i2c_port,
    uint8_t scl_pin,
    uint8_t sda_pin,
    uint32_t i2c_freq,
    int8_t reset_pin,
    int8_t pwdn_pin) {
  
  if (this->initialized_) {
    ESP_LOGW(TAG, "Système vidéo déjà initialisé");
    return true;
  }
  
  ESP_LOGI(TAG, "Configuration système vidéo:");
  ESP_LOGI(TAG, "  I2C: Port=%d, SCL=%d, SDA=%d, Freq=%u Hz", 
           i2c_port, scl_pin, sda_pin, i2c_freq);
  
  // Configuration SCCB (I2C pour capteur)
  esp_video_init_sccb_config_t sccb_config = {};
  sccb_config.init_sccb = true;
  sccb_config.i2c_config.port = i2c_port;
  sccb_config.i2c_config.scl_pin = scl_pin;
  sccb_config.i2c_config.sda_pin = sda_pin;
  sccb_config.freq = i2c_freq;
  
  // Configuration CSI
  esp_video_init_csi_config_t csi_config = {};
  csi_config.sccb_config = sccb_config;
  csi_config.reset_pin = reset_pin;
  csi_config.pwdn_pin = pwdn_pin;
  
  // Configuration globale
  esp_video_init_config_t init_config = {};
  init_config.csi = &csi_config;
  init_config.dvp = nullptr;
  init_config.jpeg = nullptr;
  init_config.isp = nullptr;
  
  // Initialiser le système
  esp_err_t ret = esp_video_init(&init_config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "❌ Échec esp_video_init: %s", esp_err_to_name(ret));
    return false;
  }
  
  // Ouvrir les devices
  this->csi_video_ = esp_video_open(ESP_VIDEO_MIPI_CSI_DEVICE_NAME);
  if (!this->csi_video_) {
    ESP_LOGE(TAG, "❌ Échec ouverture CSI device");
    return false;
  }
  
  this->isp_video_ = esp_video_open(ESP_VIDEO_ISP1_DEVICE_NAME);
  if (!this->isp_video_) {
    ESP_LOGE(TAG, "❌ Échec ouverture ISP device");
    esp_video_close(this->csi_video_);
    this->csi_video_ = nullptr;
    return false;
  }
  
  this->initialized_ = true;
  ESP_LOGI(TAG, "✅ Système ESP-VIDEO initialisé");
  
  return true;
}

void ESPVideoWrapper::dump_config() {
  ESP_LOGCONFIG(TAG, "ESP-VIDEO Wrapper:");
  ESP_LOGCONFIG(TAG, "  Initialisé: %s", this->initialized_ ? "OUI" : "NON");
  ESP_LOGCONFIG(TAG, "  Device CSI: %p", this->csi_video_);
  ESP_LOGCONFIG(TAG, "  Device ISP: %p", this->isp_video_);
}

}  // namespace esp_video_wrapper
}  // namespace esphome

#endif  // USE_ESP32_VARIANT_ESP32P4
