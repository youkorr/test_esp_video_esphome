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
}

// Headers POSIX pour V4L2
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

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

struct ResolutionInfo {
  uint16_t width;
  uint16_t height;
};

/**
 * @brief Composant Tab5 Camera utilisant ESP-Video (V4L2 API)
 * 
 * Architecture:
 *   SC202CS → I2C config
 *   SC202CS → MIPI-CSI → ESP-Video (/dev/video0) → ISP → RGB565
 * 
 * Ce composant:
 * 1. Configure le capteur SC202CS via I2C (comme avant)
 * 2. Initialise ESP-Video avec le device CSI
 * 3. Utilise l'API V4L2 standard pour capturer les frames
 * 4. Expose les mêmes méthodes publiques que l'ancienne version
 *    (pour compatibilité avec lvgl_camera_display)
 */
class Tab5CameraV4L2 : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // ========================================================================
  // CONFIGURATION (appelée depuis __init__.py)
  // ========================================================================
  
  void set_name(const std::string &name) { this->name_ = name; }
  void set_external_clock_pin(uint8_t pin) { this->external_clock_pin_ = pin; }
  void set_external_clock_frequency(uint32_t freq) { this->external_clock_frequency_ = freq; }
  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = pin; }
  void set_resolution(CameraResolution resolution) { this->resolution_ = resolution; }
  void set_pixel_format(PixelFormat format) { this->pixel_format_ = format; }
  void set_framerate(uint8_t fps) { this->framerate_ = fps; }
  void set_flip_mirror(bool enable) { this->flip_mirror_ = enable; }
  
  // Contrôles avancés
  void set_auto_gain(bool enable) { this->auto_gain_ = enable; }
  void set_manual_gain(uint32_t gain) { this->manual_gain_ = gain; }
  void set_auto_exposure(bool enable) { this->auto_exposure_ = enable; }
  void set_manual_exposure(uint32_t exposure) { this->manual_exposure_ = exposure; }

  // ========================================================================
  // API PUBLIQUE (compatible avec lvgl_camera_display)
  // ========================================================================
  
  /**
   * @brief Démarre le streaming vidéo
   * @return true si succès, false sinon
   */
  bool start_streaming();
  
  /**
   * @brief Arrête le streaming vidéo
   * @return true si succès, false sinon
   */
  bool stop_streaming();
  
  /**
   * @brief Vérifie si le streaming est actif
   * @return true si en streaming
   */
  bool is_streaming() const { return this->streaming_; }
  
  /**
   * @brief Capture une frame (dequeue depuis ESP-Video)
   * @return true si une frame est disponible, false sinon
   */
  bool capture_frame();
  
  /**
   * @brief Obtient le pointeur vers les données de l'image courante
   * @return Pointeur vers les données RGB565
   */
  uint8_t* get_image_data() { return this->current_frame_buffer_; }
  
  /**
   * @brief Obtient la taille de l'image en bytes
   * @return Taille en bytes (width * height * 2 pour RGB565)
   */
  size_t get_image_size() const { return this->frame_buffer_size_; }
  
  /**
   * @brief Obtient la largeur de l'image
   * @return Largeur en pixels
   */
  uint16_t get_image_width() const;
  
  /**
   * @brief Obtient la hauteur de l'image
   * @return Hauteur en pixels
   */
  uint16_t get_image_height() const;

 protected:
  // ========================================================================
  // CONFIGURATION
  // ========================================================================
  
  std::string name_{"Tab5 Camera"};
  uint8_t external_clock_pin_{36};
  uint32_t external_clock_frequency_{24000000};
  GPIOPin *reset_pin_{nullptr};
  
  CameraResolution resolution_{RESOLUTION_720P};
  PixelFormat pixel_format_{PIXEL_FORMAT_RGB565};
  uint8_t framerate_{30};
  bool flip_mirror_{false};
  
  // Contrôles avancés
  bool auto_gain_{true};
  uint32_t manual_gain_{0};
  bool auto_exposure_{true};
  uint32_t manual_exposure_{0x300};
  
  // ========================================================================
  // ÉTAT
  // ========================================================================
  
  bool initialized_{false};
  bool streaming_{false};
  
  // Buffer de la frame courante
  uint8_t *current_frame_buffer_{nullptr};
  size_t frame_buffer_size_{0};
  
  // Stats
  uint32_t total_frames_captured_{0};
  uint32_t last_fps_report_time_{0};
  
#ifdef USE_ESP32_VARIANT_ESP32P4
  
  // ========================================================================
  // ESP-VIDEO / V4L2
  // ========================================================================
  
  // File descriptor du device video
  int video_fd_{-1};
  
  // Buffers V4L2
  struct v4l2_buffer *v4l2_buffers_{nullptr};
  uint32_t buffer_count_{0};
  uint32_t current_buffer_index_{0};
  
  // Mapping mémoire des buffers
  struct BufferMapping {
    void *start;
    size_t length;
  };
  BufferMapping *buffer_mappings_{nullptr};
  
  // ========================================================================
  // MÉTHODES PRIVÉES
  // ========================================================================
  
  /**
   * @brief Initialise ESP-Video avec le device CSI
   * @return true si succès
   */
  bool init_esp_video_();
  
  /**
   * @brief Configure le format vidéo via V4L2
   * @return true si succès
   */
  bool configure_format_();
  
  /**
   * @brief Setup les buffers V4L2 (mmap)
   * @return true si succès
   */
  bool setup_buffers_();
  
  /**
   * @brief Applique les contrôles (gain, exposition, flip/mirror)
   * @return true si succès
   */
  bool apply_controls_();
  
  /**
   * @brief Obtient les infos de résolution
   * @return Structure avec width/height
   */
  ResolutionInfo get_resolution_info_() const;
  
#endif
};

}  // namespace tab5_camera
}  // namespace esphome

