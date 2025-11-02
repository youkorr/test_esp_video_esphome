#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"

#ifdef USE_ESP32_VARIANT_ESP32P4

// Headers ESP-Video (disponibles grâce au composant esp_video)
extern "C" {
  #include "esp_video_init.h"
  #include "esp_video_device.h"
  #include "linux/videodev2.h"
  #include "esp_cam_sensor.h"
  #include "driver/i2c_master.h"
}

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#endif

namespace esphome {
namespace tab5_camera {

enum CameraResolution {
  RESOLUTION_720P = 0,
};

enum PixelFormat {
  PIXEL_FORMAT_RGB565 = 0,
};

/**
 * @brief Tab5 Camera avec ESP-Video + Driver SC202CS complet
 * 
 * Architecture:
 *   SC202CS (I2C config) → RAW8 → MIPI-CSI → ESP-Video → ISP → RGB565 → V4L2
 */
class Tab5CameraV4L2 : public Component, public i2c::I2CDevice {
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
  void set_resolution(CameraResolution resolution) { this->resolution_ = resolution; }
  void set_pixel_format(PixelFormat format) { this->pixel_format_ = format; }
  void set_framerate(uint8_t fps) { this->framerate_ = fps; }
  void set_flip_mirror(bool enable) { this->flip_mirror_ = enable; }
  void set_auto_gain(bool enable) { this->auto_gain_ = enable; }
  void set_manual_gain(uint32_t gain) { this->manual_gain_ = gain; }
  void set_auto_exposure(bool enable) { this->auto_exposure_ = enable; }
  void set_manual_exposure(uint32_t exposure) { this->manual_exposure_ = exposure; }

  // API publique (compatible avec lvgl_camera_display)
  bool start_streaming();
  bool stop_streaming();
  bool is_streaming() const { return this->streaming_; }
  bool capture_frame();
  uint8_t* get_image_data() { return this->current_frame_buffer_; }
  size_t get_image_size() const { return this->frame_buffer_size_; }
  uint16_t get_image_width() const { return 1280; }
  uint16_t get_image_height() const { return 720; }

 protected:
  std::string name_{"Tab5 Camera"};
  uint8_t external_clock_pin_{36};
  uint32_t external_clock_frequency_{24000000};
  GPIOPin *reset_pin_{nullptr};
  
  CameraResolution resolution_{RESOLUTION_720P};
  PixelFormat pixel_format_{PIXEL_FORMAT_RGB565};
  uint8_t framerate_{30};
  bool flip_mirror_{false};
  
  bool auto_gain_{true};
  uint32_t manual_gain_{0};
  bool auto_exposure_{true};
  uint32_t manual_exposure_{0x300};
  
  bool initialized_{false};
  bool streaming_{false};
  uint8_t *current_frame_buffer_{nullptr};
  size_t frame_buffer_size_{0};
  uint32_t total_frames_captured_{0};
  uint32_t last_fps_report_time_{0};
  
#ifdef USE_ESP32_VARIANT_ESP32P4
  
  // V4L2
  int video_fd_{-1};
  struct v4l2_buffer *v4l2_buffers_{nullptr};
  uint32_t buffer_count_{0};
  uint32_t current_buffer_index_{0};
  
  struct BufferMapping {
    void *start;
    size_t length;
  };
  BufferMapping *buffer_mappings_{nullptr};
  
  // SC202CS
  esp_cam_sensor_device_t *sensor_device_{nullptr};
  
  // Méthodes privées
  bool init_sensor_sc202cs_();
  bool configure_sensor_params_();
  bool init_esp_video_();
  bool configure_format_v4l2_();
  bool setup_buffers_v4l2_();
  bool start_sensor_streaming_();
  
  // Helpers I2C pour SC202CS (via I2CDevice ESPHome)
  bool sc202cs_write_reg_(uint16_t reg, uint8_t value);
  uint8_t sc202cs_read_reg_(uint16_t reg, bool *success = nullptr);
  
#endif
};

}  // namespace tab5_camera
}  // namespace esphome


