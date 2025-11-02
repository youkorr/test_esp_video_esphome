#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_ESP32_VARIANT_ESP32P4

extern "C" {
#include "esp_video.h"
#include "esp_video_init.h"
#include "esp_video_device.h"
#include "esp_video_ioctl.h"
#include "linux/videodev2.h"
}

namespace esphome {
namespace esp_video_wrapper {

class ESPVideoWrapper : public Component {
 public:
  void setup() override;
  void loop() override {}
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  
  // API publique pour tab5_camera
  struct esp_video *get_csi_device() { return this->csi_video_; }
  struct esp_video *get_isp_device() { return this->isp_video_; }
  
  bool initialize_video_system(
    uint8_t i2c_port,
    uint8_t scl_pin,
    uint8_t sda_pin,
    uint32_t i2c_freq,
    int8_t reset_pin,
    int8_t pwdn_pin
  );
  
 protected:
  struct esp_video *csi_video_{nullptr};
  struct esp_video *isp_video_{nullptr};
  bool initialized_{false};
};

}  // namespace esp_video_wrapper
}  // namespace esphome

#endif  // USE_ESP32_VARIANT_ESP32P4
