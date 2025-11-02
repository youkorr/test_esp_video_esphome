#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"

#ifdef USE_ESP32_VARIANT_ESP32P4
// ✅ Utiliser le VRAI ESP-Video framework d'Espressif
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

  // Configuration de base
  void set_name(const std::string &name) { this->name_ = name; }
  void set_external_clock_pin(uint8_t pin) { this->external_clock_pin_ = pin; }
  void set_external_clock_frequency(uint32_t freq) { this->external_clock_frequency_ = freq; }
  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = pin; }
  void set_pwdn_pin(GPIOPin *pin) { this->pwdn_pin_ = pin; }
  void set_sensor_address(uint8_t address) { this->sensor_address_ = address; }
  void set_resolution(CameraResolution resolution) { this->resolution_ = resolution; }
  void set_pixel_format(PixelFormat format) { this->pixel_format_ = format; }
  void set_jpeg_quality(uint8_t quality) { this->jpeg_quality_ = quality; }
  void set_framerate(uint8_t fps) { this->framerate_ = fps; }
  void set_flip_mirror(bool enable) { this->flip_mirror_ = enable; }

  // Contrôle avancé des paramètres
  void set_auto_gain(bool enable) { this->auto_gain_ = enable; }
  void set_manual_gain(uint32_t gain_index) { this->manual_gain_index_ = gain_index; }
  void set_auto_exposure(bool enable) { this->auto_exposure_ = enable; }
  void set_manual_exposure(uint32_t exposure_val) { this->manual_exposure_val_ = exposure_val; }

  // Opérations de base
  bool capture_frame();
  bool start_streaming();
  bool stop_streaming();
  bool is_streaming() const { return this->streaming_; }
  
  // Accès aux données
  uint8_t* get_image_data() { return this->current_frame_buffer_; }
  size_t get_image_size() const { return this->frame_buffer_size_; }
  uint16_t get_image_width() const;
  uint16_t get_image_height() const;

  // Contrôle des paramètres en temps réel
  bool set_gain(uint32_t gain_index);
  bool set_exposure(uint32_t exposure_val);
  uint32_t get_current_gain() const { return this->current_gain_index_; }
  uint32_t get_current_exposure() const { return this->current_exposure_val_; }

 protected:
  // Configuration matérielle
  uint8_t external_clock_pin_{36};
  uint32_t external_clock_frequency_{24000000};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *pwdn_pin_{nullptr};
  uint8_t sensor_address_{0x36};
  std::string name_{"Tab5 Camera"};
  
  // Configuration de l'image
  CameraResolution resolution_{RESOLUTION_720P};
  PixelFormat pixel_format_{PIXEL_FORMAT_RGB565};
  uint8_t jpeg_quality_{10};
  uint8_t framerate_{30};
  bool flip_mirror_{false};

  // Contrôle avancé
  bool auto_gain_{true};
  uint32_t manual_gain_index_{0};
  bool auto_exposure_{true};
  uint32_t manual_exposure_val_{0x4dc};
  
  // État actuel des paramètres
  uint32_t current_gain_index_{0};
  uint32_t current_exposure_val_{0x4dc};
  
  // État de la caméra
  bool initialized_{false};
  bool streaming_{false};
  
  // Buffers d'image
  uint8_t *current_frame_buffer_{nullptr};
  size_t frame_buffer_size_{0};
  
#ifdef USE_ESP32_VARIANT_ESP32P4
  // ✅ Handles ESP-Video (V4L2-like)
  int video_fd_{-1};                    // File descriptor du device /dev/video0
  struct v4l2_buffer *v4l2_buffers_{nullptr};  // Buffers V4L2
  uint32_t buffer_count_{0};            // Nombre de buffers
  uint32_t current_buffer_index_{0};    // Index du buffer courant
  
  // Fonctions d'initialisation ESP-Video
  bool init_esp_video_();
  bool configure_format_();
  bool setup_buffers_();
  bool apply_controls_();
#endif
};

}  // namespace tab5_camera
}  // namespace esphome

