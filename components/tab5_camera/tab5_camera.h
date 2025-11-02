#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"

#ifdef USE_ESP32_VARIANT_ESP32P4
// ✅ Utiliser le vrai ESP-Video
extern "C" {
  #include "esp_video_init.h"
  #include "esp_video_device.h"
  #include "esp_video_ioctl.h"
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
  PIXEL_FORMAT_RAW8 = 2,
  PIXEL_FORMAT_JPEG = 3,
};

class Tab5Camera : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Configuration
  void set_name(const std::string &name) { this->name_ = name; }
  void set_external_clock_pin(uint8_t pin) { this->external_clock_pin_ = pin; }
  void set_external_clock_frequency(uint32_t freq) { this->external_clock_frequency_ = freq; }
  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = pin; }
  void set_pwdn_pin(GPIOPin *pin) { this->pwdn_pin_ = pin; }
  void set_sensor_address(uint8_t address) { this->sensor_address_ = address; }
  void set_resolution(CameraResolution resolution) { this->resolution_ = resolution; }
  void set_pixel_format(PixelFormat format) { this->pixel_format_ = format; }
  void set_framerate(uint8_t fps) { this->framerate_ = fps; }
  void set_flip_mirror(bool enable) { this->flip_mirror_ = enable; }

  // Contrôle avancé
  void set_auto_gain(bool enable) { this->auto_gain_ = enable; }
  void set_manual_gain(uint32_t gain) { this->manual_gain_ = gain; }
  void set_auto_exposure(bool enable) { this->auto_exposure_ = enable; }
  void set_manual_exposure(uint32_t exposure) { this->manual_exposure_ = exposure; }

  // Opérations
  bool capture_frame();
  bool start_streaming();
  bool stop_streaming();
  bool is_streaming() const { return this->streaming_; }
  
  // Accès aux données
  uint8_t* get_image_data() { return this->current_frame_buffer_; }
  size_t get_image_size() const { return this->frame_buffer_size_; }
  uint16_t get_image_width() const;
  uint16_t get_image_height() const;

 protected:
  // Configuration matérielle
  uint8_t external_clock_pin_{36};
  uint32_t external_clock_frequency_{24000000};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *pwdn_pin_{nullptr};
  uint8_t sensor_address_{0x36};
  std::string name_{"Tab5 Camera"};
  
  // Configuration image
  CameraResolution resolution_{RESOLUTION_720P};
  PixelFormat pixel_format_{PIXEL_FORMAT_RGB565};
  uint8_t framerate_{30};
  bool flip_mirror_{false};
  
  // Contrôle avancé
  bool auto_gain_{true};
  uint32_t manual_gain_{0};
  bool auto_exposure_{true};
  uint32_t manual_exposure_{0x4dc};
  
  // État
  bool initialized_{false};
  bool streaming_{false};
  
  // Buffers
  uint8_t *current_frame_buffer_{nullptr};
  size_t frame_buffer_size_{0};
  
#ifdef USE_ESP32_VARIANT_ESP32P4
  // ✅ Handle ESP-Video
  int video_fd_{-1};  // File descriptor du device /dev/video0
  struct v4l2_buffer *buffers_{nullptr};
  uint32_t buffer_count_{0};
  
  // Fonctions d'initialisation
  bool init_esp_video_();
  bool configure_format_();
  bool setup_buffers_();
  bool apply_controls_();
#endif
};

}  // namespace tab5_camera
}  // namespace esphome

