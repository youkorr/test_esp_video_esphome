#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"

#ifdef USE_ESP32_VARIANT_ESP32P4
#include "esphome/components/esp_video_wrapper/esp_video_wrapper.h"

extern "C" {
#include "linux/videodev2.h"
}
#endif

namespace esphome {
namespace tab5_camera {

enum CameraResolution {
  RESOLUTION_VGA = 0,
  RESOLUTION_720P = 1,
  RESOLUTION_1080P = 2,
};

enum PixelFormat {
  PIXEL_FORMAT_RGB565 = 0,
  PIXEL_FORMAT_YUV422 = 1,
};

struct CameraResolutionInfo {
  uint16_t width;
  uint16_t height;
};

class Tab5Camera : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Configuration
  void set_esp_video_wrapper(esp_video_wrapper::ESPVideoWrapper *wrapper) {
    this->video_wrapper_ = wrapper;
  }
  void set_resolution(CameraResolution resolution) { this->resolution_ = resolution; }
  void set_pixel_format(PixelFormat format) { this->pixel_format_ = format; }
  void set_framerate(uint8_t fps) { this->framerate_ = fps; }
  void set_flip_mirror(bool enable) { this->flip_mirror_ = enable; }
  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = pin; }

  // Opérations
  bool capture_frame();
  bool start_streaming();
  bool stop_streaming();
  bool is_streaming() const { return this->streaming_; }
  
  // Accès données
  uint8_t* get_image_data() { return this->current_frame_buffer_; }
  uint16_t get_image_width() const;
  uint16_t get_image_height() const;

 protected:
  esp_video_wrapper::ESPVideoWrapper *video_wrapper_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  
  CameraResolution resolution_{RESOLUTION_720P};
  PixelFormat pixel_format_{PIXEL_FORMAT_RGB565};
  uint8_t framerate_{30};
  bool flip_mirror_{false};
  
  bool initialized_{false};
  bool streaming_{false};
  
  uint8_t *frame_buffers_[2]{nullptr, nullptr};
  uint8_t *current_frame_buffer_{nullptr};
  size_t frame_buffer_size_{0};
  int current_buffer_index_{0};
  
#ifdef USE_ESP32_VARIANT_ESP32P4
  int csi_fd_{-1};
  int isp_fd_{-1};
  
  bool init_with_esp_video_();
  bool configure_csi_format_();
  bool configure_isp_format_();
  bool allocate_buffers_();
  CameraResolutionInfo get_resolution_info_() const;
#endif
};

}  // namespace tab5_camera
}  // namespace esphome
