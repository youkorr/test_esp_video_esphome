#include "tab5_camera.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32_VARIANT_ESP32P4

// ============================================================================
// VOTRE DRIVER SC202CS COMPLET (réutilisé tel quel)
// ============================================================================

extern "C" {

static const char *SC202CS_TAG = "sc202cs";

typedef struct {
    uint16_t reg;
    uint8_t val;
} sc202cs_reginfo_t;

#define SC202CS_REG_END   0xffff
#define SC202CS_REG_DELAY 0xfffe
#define SC202CS_REG_SENSOR_ID_H 0x3107
#define SC202CS_REG_SENSOR_ID_L 0x3108
#define SC202CS_REG_SLEEP_MODE  0x0100
#define SC202CS_REG_DIG_COARSE_GAIN 0x3e06
#define SC202CS_REG_DIG_FINE_GAIN   0x3e07
#define SC202CS_REG_ANG_GAIN        0x3e09
#define SC202CS_REG_SHUTTER_TIME_H 0x3e00
#define SC202CS_REG_SHUTTER_TIME_M 0x3e01
#define SC202CS_REG_SHUTTER_TIME_L 0x3e02

#define SC202CS_PID         0xeb52
#define SC202CS_SENSOR_NAME "SC202CS"

#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif
#define delay_ms(ms) vTaskDelay((ms > portTICK_PERIOD_MS ? ms / portTICK_PERIOD_MS : 1))

// ✅ LISTE COMPLÈTE DES REGISTRES 720P@30FPS
static const sc202cs_reginfo_t init_reglist_1280x720_30fps[] = {
    {0x0103, 0x01},
    {SC202CS_REG_SLEEP_MODE, 0x00},
    {0x36e9, 0x80}, {0x36ea, 0x06}, {0x36eb, 0x0a}, {0x36ec, 0x01},
    {0x36ed, 0x18}, {0x36e9, 0x24}, {0x301f, 0x18}, {0x3031, 0x08},
    {0x3037, 0x00}, {0x3200, 0x00}, {0x3201, 0xa0}, {0x3202, 0x00},
    {0x3203, 0xf0}, {0x3204, 0x05}, {0x3205, 0xa7}, {0x3206, 0x03},
    {0x3207, 0xc7}, {0x3208, 0x05}, {0x3209, 0x00}, {0x320a, 0x02},
    {0x320b, 0xd0}, {0x3210, 0x00}, {0x3211, 0x04}, {0x3212, 0x00},
    {0x3213, 0x04}, {0x3301, 0xff}, {0x3304, 0x68}, {0x3306, 0x40},
    {0x3308, 0x08}, {0x3309, 0xa8}, {0x330b, 0xd0}, {0x330c, 0x18},
    {0x330d, 0xff}, {0x330e, 0x20}, {0x331e, 0x59}, {0x331f, 0x99},
    {0x3333, 0x10}, {0x335e, 0x06}, {0x335f, 0x08}, {0x3364, 0x1f},
    {0x337c, 0x02}, {0x337d, 0x0a}, {0x338f, 0xa0}, {0x3390, 0x01},
    {0x3391, 0x03}, {0x3392, 0x1f}, {0x3393, 0xff}, {0x3394, 0xff},
    {0x3395, 0xff}, {0x33a2, 0x04}, {0x33ad, 0x0c}, {0x33b1, 0x20},
    {0x33b3, 0x38}, {0x33f9, 0x40}, {0x33fb, 0x48}, {0x33fc, 0x0f},
    {0x33fd, 0x1f}, {0x349f, 0x03}, {0x34a6, 0x03}, {0x34a7, 0x1f},
    {0x34a8, 0x38}, {0x34a9, 0x30}, {0x34ab, 0xd0}, {0x34ad, 0xd8},
    {0x34f8, 0x1f}, {0x34f9, 0x20}, {0x3630, 0xa0}, {0x3631, 0x92},
    {0x3632, 0x64}, {0x3633, 0x43}, {0x3637, 0x49}, {0x363a, 0x85},
    {0x363c, 0x0f}, {0x3650, 0x31}, {0x3670, 0x0d}, {0x3674, 0xc0},
    {0x3675, 0xa0}, {0x3676, 0xa0}, {0x3677, 0x92}, {0x3678, 0x96},
    {0x3679, 0x9a}, {0x367c, 0x03}, {0x367d, 0x0f}, {0x367e, 0x01},
    {0x367f, 0x0f}, {0x3698, 0x83}, {0x3699, 0x86}, {0x369a, 0x8c},
    {0x369b, 0x94}, {0x36a2, 0x01}, {0x36a3, 0x03}, {0x36a4, 0x07},
    {0x36ae, 0x0f}, {0x36af, 0x1f}, {0x36bd, 0x22}, {0x36be, 0x22},
    {0x36bf, 0x22}, {0x36d0, 0x01}, {0x370f, 0x02}, {0x3721, 0x6c},
    {0x3722, 0x8d}, {0x3725, 0xc5}, {0x3727, 0x14}, {0x3728, 0x04},
    {0x37b7, 0x04}, {0x37b8, 0x04}, {0x37b9, 0x06}, {0x37bd, 0x07},
    {0x37be, 0x0f}, {0x3901, 0x02}, {0x3903, 0x40}, {0x3905, 0x8d},
    {0x3907, 0x00}, {0x3908, 0x41}, {0x391f, 0x41}, {0x3933, 0x80},
    {0x3934, 0x02}, {0x3937, 0x6f}, {0x393a, 0x01}, {0x393d, 0x01},
    {0x393e, 0xc0}, {0x39dd, 0x41}, {0x3e00, 0x00}, {0x3e01, 0x4d},
    {0x3e02, 0xc0}, {0x3e06, 0x00}, {0x3e07, 0x80}, {0x3e09, 0x00},
    {0x4509, 0x28}, {0x450d, 0x61},
    {SC202CS_REG_END, 0x00},
};

// ✅ TABLES DE GAIN COMPLÈTES (192 entrées)
typedef struct {
    uint8_t dgain_fine;
    uint8_t dgain_coarse;
    uint8_t analog_gain;
} sc202cs_gain_t;

static const uint32_t sc202cs_abs_gain_val_map[] = {
    1000,  1031,  1063,  1094,  1125,  1156,  1188,  1219,
    1250,  1281,  1313,  1344,  1375,  1406,  1438,  1469,
    1500,  1531,  1563,  1594,  1625,  1656,  1688,  1719,
    1750,  1781,  1813,  1844,  1875,  1906,  1938,  1969,
    2000,  2062,  2126,  2188,  2250,  2312,  2376,  2438,
    2500,  2562,  2626,  2688,  2750,  2812,  2876,  2938,
    3000,  3062,  3126,  3188,  3250,  3312,  3376,  3438,
    3500,  3562,  3626,  3688,  3750,  3812,  3876,  3938,
    4000,  4124,  4252,  4376,  4500,  4624,  4752,  4876,
    5000,  5124,  5252,  5376,  5500,  5624,  5752,  5876,
    6000,  6124,  6252,  6376,  6500,  6624,  6752,  6876,
    7000,  7124,  7252,  7376,  7500,  7624,  7752,  7876,
    8000,  8248,  8504,  8752,  9000,  9248,  9504,  9752,
    10000, 10248, 10504, 10752, 11000, 11248, 11504, 11752,
    12000, 12248, 12504, 12752, 13000, 13248, 13504, 13752,
    14000, 14248, 14504, 14752, 15000, 15248, 15504, 15752,
    16000, 16496, 17008, 17504, 18000, 18496, 19008, 19504,
    20000, 20496, 21008, 21504, 22000, 22496, 23008, 23504,
    24000, 24496, 25008, 25504, 26000, 26496, 27008, 27504,
    28000, 28496, 29008, 29504, 30000, 30496, 31008, 31504,
    32000, 33008, 34000, 35008, 36000, 37008, 38000, 39008,
    40000, 41008, 42000, 43008, 44000, 45008, 46000, 47008,
    48000, 49008, 50000, 51008, 52000, 53008, 54000, 55008,
    56000, 57008, 58000, 59008, 60000, 61008, 62000, 63008,
};

static const sc202cs_gain_t sc202cs_gain_map[] = {
    {0x80, 0x00, 0x00}, {0x84, 0x00, 0x00}, {0x88, 0x00, 0x00}, {0x8c, 0x00, 0x00},
    {0x90, 0x00, 0x00}, {0x94, 0x00, 0x00}, {0x98, 0x00, 0x00}, {0x9c, 0x00, 0x00},
    {0xa0, 0x00, 0x00}, {0xa4, 0x00, 0x00}, {0xa8, 0x00, 0x00}, {0xac, 0x00, 0x00},
    {0xb0, 0x00, 0x00}, {0xb4, 0x00, 0x00}, {0xb8, 0x00, 0x00}, {0xbc, 0x00, 0x00},
    {0xc0, 0x00, 0x00}, {0xc4, 0x00, 0x00}, {0xc8, 0x00, 0x00}, {0xcc, 0x00, 0x00},
    {0xd0, 0x00, 0x00}, {0xd4, 0x00, 0x00}, {0xd8, 0x00, 0x00}, {0xdc, 0x00, 0x00},
    {0xe0, 0x00, 0x00}, {0xe4, 0x00, 0x00}, {0xe8, 0x00, 0x00}, {0xec, 0x00, 0x00},
    {0xf0, 0x00, 0x00}, {0xf4, 0x00, 0x00}, {0xf8, 0x00, 0x00}, {0xfc, 0x00, 0x00},
    {0x80, 0x00, 0x01}, {0x84, 0x00, 0x01}, {0x88, 0x00, 0x01}, {0x8c, 0x00, 0x01},
    {0x90, 0x00, 0x01}, {0x94, 0x00, 0x01}, {0x98, 0x00, 0x01}, {0x9c, 0x00, 0x01},
    {0xa0, 0x00, 0x01}, {0xa4, 0x00, 0x01}, {0xa8, 0x00, 0x01}, {0xac, 0x00, 0x01},
    {0xb0, 0x00, 0x01}, {0xb4, 0x00, 0x01}, {0xb8, 0x00, 0x01}, {0xbc, 0x00, 0x01},
    {0xc0, 0x00, 0x01}, {0xc4, 0x00, 0x01}, {0xc8, 0x00, 0x01}, {0xcc, 0x00, 0x01},
    {0xd0, 0x00, 0x01}, {0xd4, 0x00, 0x01}, {0xd8, 0x00, 0x01}, {0xdc, 0x00, 0x01},
    {0xe0, 0x00, 0x01}, {0xe4, 0x00, 0x01}, {0xe8, 0x00, 0x01}, {0xec, 0x00, 0x01},
    {0xf0, 0x00, 0x01}, {0xf4, 0x00, 0x01}, {0xf8, 0x00, 0x01}, {0xfc, 0x00, 0x01},
    {0x80, 0x00, 0x03}, {0x84, 0x00, 0x03}, {0x88, 0x00, 0x03}, {0x8c, 0x00, 0x03},
    {0x90, 0x00, 0x03}, {0x94, 0x00, 0x03}, {0x98, 0x00, 0x03}, {0x9c, 0x00, 0x03},
    {0xa0, 0x00, 0x03}, {0xa4, 0x00, 0x03}, {0xa8, 0x00, 0x03}, {0xac, 0x00, 0x03},
    {0xb0, 0x00, 0x03}, {0xb4, 0x00, 0x03}, {0xb8, 0x00, 0x03}, {0xbc, 0x00, 0x03},
    {0xc0, 0x00, 0x03}, {0xc4, 0x00, 0x03}, {0xc8, 0x00, 0x03}, {0xcc, 0x00, 0x03},
    {0xd0, 0x00, 0x03}, {0xd4, 0x00, 0x03}, {0xd8, 0x00, 0x03}, {0xdc, 0x00, 0x03},
    {0xe0, 0x00, 0x03}, {0xe4, 0x00, 0x03}, {0xe8, 0x00, 0x03}, {0xec, 0x00, 0x03},
    {0xf0, 0x00, 0x03}, {0xf4, 0x00, 0x03}, {0xf8, 0x00, 0x03}, {0xfc, 0x00, 0x03},
    {0x80, 0x00, 0x07}, {0x84, 0x00, 0x07}, {0x88, 0x00, 0x07}, {0x8c, 0x00, 0x07},
    {0x90, 0x00, 0x07}, {0x94, 0x00, 0x07}, {0x98, 0x00, 0x07}, {0x9c, 0x00, 0x07},
    {0xa0, 0x00, 0x07}, {0xa4, 0x00, 0x07}, {0xa8, 0x00, 0x07}, {0xac, 0x00, 0x07},
    {0xb0, 0x00, 0x07}, {0xb4, 0x00, 0x07}, {0xb8, 0x00, 0x07}, {0xbc, 0x00, 0x07},
    {0xc0, 0x00, 0x07}, {0xc4, 0x00, 0x07}, {0xc8, 0x00, 0x07}, {0xcc, 0x00, 0x07},
    {0xd0, 0x00, 0x07}, {0xd4, 0x00, 0x07}, {0xd8, 0x00, 0x07}, {0xdc, 0x00, 0x07},
    {0xe0, 0x00, 0x07}, {0xe4, 0x00, 0x07}, {0xe8, 0x00, 0x07}, {0xec, 0x00, 0x07},
    {0xf0, 0x00, 0x07}, {0xf4, 0x00, 0x07}, {0xf8, 0x00, 0x07}, {0xfc, 0x00, 0x07},
    {0x80, 0x00, 0x0f}, {0x84, 0x00, 0x0f}, {0x88, 0x00, 0x0f}, {0x8c, 0x00, 0x0f},
    {0x90, 0x00, 0x0f}, {0x94, 0x00, 0x0f}, {0x98, 0x00, 0x0f}, {0x9c, 0x00, 0x0f},
    {0xa0, 0x00, 0x0f}, {0xa4, 0x00, 0x0f}, {0xa8, 0x00, 0x0f}, {0xac, 0x00, 0x0f},
    {0xb0, 0x00, 0x0f}, {0xb4, 0x00, 0x0f}, {0xb8, 0x00, 0x0f}, {0xbc, 0x00, 0x0f},
    {0xc0, 0x00, 0x0f}, {0xc4, 0x00, 0x0f}, {0xc8, 0x00, 0x0f}, {0xcc, 0x00, 0x0f},
    {0xd0, 0x00, 0x0f}, {0xd4, 0x00, 0x0f}, {0xd8, 0x00, 0x0f}, {0xdc, 0x00, 0x0f},
    {0xe0, 0x00, 0x0f}, {0xe4, 0x00, 0x0f}, {0xe8, 0x00, 0x0f}, {0xec, 0x00, 0x0f},
    {0xf0, 0x00, 0x0f}, {0xf4, 0x00, 0x0f}, {0xf8, 0x00, 0x0f}, {0xfc, 0x00, 0x0f},
    {0x80, 0x01, 0x0f}, {0x84, 0x01, 0x0f}, {0x88, 0x01, 0x0f}, {0x8c, 0x01, 0x0f},
    {0x90, 0x01, 0x0f}, {0x94, 0x01, 0x0f}, {0x98, 0x01, 0x0f}, {0x9c, 0x01, 0x0f},
    {0xa0, 0x01, 0x0f}, {0xa4, 0x01, 0x0f}, {0xa8, 0x01, 0x0f}, {0xac, 0x01, 0x0f},
    {0xb0, 0x01, 0x0f}, {0xb4, 0x01, 0x0f}, {0xb8, 0x01, 0x0f}, {0xbc, 0x01, 0x0f},
    {0xc0, 0x01, 0x0f}, {0xc4, 0x01, 0x0f}, {0xc8, 0x01, 0x0f}, {0xcc, 0x01, 0x0f},
    {0xd0, 0x01, 0x0f}, {0xd4, 0x01, 0x0f}, {0xd8, 0x01, 0x0f}, {0xdc, 0x01, 0x0f},
    {0xe0, 0x01, 0x0f}, {0xe4, 0x01, 0x0f}, {0xe8, 0x01, 0x0f}, {0xec, 0x01, 0x0f},
    {0xf0, 0x01, 0x0f}, {0xf4, 0x01, 0x0f}, {0xf8, 0x01, 0x0f}, {0xfc, 0x01, 0x0f},
};

// Macros exposition
#define SC202CS_FETCH_EXP_H(val) (((val) >> 12) & 0xF)
#define SC202CS_FETCH_EXP_M(val) (((val) >> 4) & 0xFF)
#define SC202CS_FETCH_EXP_L(val) (((val)&0xF) << 4)

// Stub pour compatibilité mais inutilisé avec ESP-Video
typedef struct {} sc202cs_para_t;
struct sc202cs_cam { sc202cs_para_t sc202cs_para; };

} // extern "C"

// ============================================================================
// IMPLÉMENTATION TAB5 CAMERA AVEC ESP-VIDEO
// ============================================================================

namespace esphome {
namespace tab5_camera {

static const char *const TAG = "tab5_camera";

// Helpers I2C
bool Tab5CameraV4L2::sc202cs_write_reg_(uint16_t reg, uint8_t value) {
  uint8_t data[3] = {
    (uint8_t)(reg >> 8),
    (uint8_t)(reg & 0xFF),
    value
  };
  return this->write(data, 3) == i2c::ERROR_OK;
}

uint8_t Tab5CameraV4L2::sc202cs_read_reg_(uint16_t reg, bool *success) {
  uint8_t addr[2] = {(uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF)};
  uint8_t value = 0;
  
  if (this->write(addr, 2) != i2c::ERROR_OK || 
      this->read(&value, 1) != i2c::ERROR_OK) {
    if (success) *success = false;
    return 0;
  }
  
  if (success) *success = true;
  return value;
}

// Init capteur avec registres complets
bool Tab5CameraV4L2::init_sensor_sc202cs_() {
  ESP_LOGI(TAG, "Init SC202CS avec registres complets...");
  
  // Vérifier ID
  bool ok;
  uint8_t id_h = this->sc202cs_read_reg_(SC202CS_REG_SENSOR_ID_H, &ok);
  if (!ok) return false;
  uint8_t id_l = this->sc202cs_read_reg_(SC202CS_REG_SENSOR_ID_L, &ok);
  if (!ok) return false;
  
  uint16_t pid = (id_h << 8) | id_l;
  ESP_LOGI(TAG, "  Chip ID: 0x%04X", pid);
  
  if (pid != SC202CS_PID) {
    ESP_LOGE(TAG, "ID invalide! (attendu 0x%04X)", SC202CS_PID);
    return false;
  }
  
  // Écrire tous les registres
  ESP_LOGI(TAG, "Écriture des registres 720p@30fps...");
  for (size_t i = 0; init_reglist_1280x720_30fps[i].reg != SC202CS_REG_END; i++) {
    uint16_t reg = init_reglist_1280x720_30fps[i].reg;
    uint8_t val = init_reglist_1280x720_30fps[i].val;
    
    if (reg == SC202CS_REG_DELAY) {
      delay(val);
    } else {
      if (!this->sc202cs_write_reg_(reg, val)) {
        ESP_LOGE(TAG, "Échec reg 0x%04X", reg);
        return false;
      }
    }
  }
  
  ESP_LOGI(TAG, "✓ SC202CS configuré (RAW8 1280x720@30fps)");
  return true;
}

// Configurer gain/exposition
bool Tab5CameraV4L2::configure_sensor_params_() {
  ESP_LOGI(TAG, "Configuration paramètres capteur...");
  
  // Flip/Mirror
  if (this->flip_mirror_) {
    this->sc202cs_write_reg_(0x3221, 0x60);  // flip+mirror bits
    ESP_LOGI(TAG, "  ✓ Flip/Mirror activé");
  }
  
  // Gain
  if (!this->auto_gain_ && this->manual_gain_ > 0) {
    uint32_t gain_idx = this->manual_gain_;
    if (gain_idx > 191) gain_idx = 191;
    
    this->sc202cs_write_reg_(SC202CS_REG_DIG_FINE_GAIN, 
                            sc202cs_gain_map[gain_idx].dgain_fine);
    this->sc202cs_write_reg_(SC202CS_REG_DIG_COARSE_GAIN, 
                            sc202cs_gain_map[gain_idx].dgain_coarse);
    this->sc202cs_write_reg_(SC202CS_REG_ANG_GAIN, 
                            sc202cs_gain_map[gain_idx].analog_gain);
    
    ESP_LOGI(TAG, "  ✓ Gain manuel[%u]: %.1fx", 
             gain_idx, sc202cs_abs_gain_val_map[gain_idx]/1000.0f);
  }
  
  // Exposition
  if (!this->auto_exposure_ && this->manual_exposure_ > 0) {
    uint32_t exp = this->manual_exposure_;
    if (exp > 0x4DC) exp = 0x4DC;
    if (exp < 0xFF) exp = 0xFF;
    
    this->sc202cs_write_reg_(SC202CS_REG_SHUTTER_TIME_H, SC202CS_FETCH_EXP_H(exp));
    this->sc202cs_write_reg_(SC202CS_REG_SHUTTER_TIME_M, SC202CS_FETCH_EXP_M(exp));
    this->sc202cs_write_reg_(SC202CS_REG_SHUTTER_TIME_L, SC202CS_FETCH_EXP_L(exp));
    
    ESP_LOGI(TAG, "  ✓ Exposition manuelle: 0x%03X", exp);
  }
  
  return true;
}

// Init ESP-Video
bool Tab5CameraV4L2::init_esp_video_() {
  ESP_LOGI(TAG, "Init ESP-Video...");
  
  esp_video_init_sccb_config_t sccb_cfg = {};
  sccb_cfg.init_sccb = false;  // On gère l'I2C via ESPHome
  
  esp_video_init_csi_config_t csi_cfg = {};
  csi_cfg.sccb_config = sccb_cfg;
  csi_cfg.reset_pin = GPIO_NUM_NC;
  csi_cfg.pwdn_pin = GPIO_NUM_NC;
  
  esp_err_t ret = esp_video_init(ESP_VIDEO_DEVICE_MIPI_CSI, &csi_cfg);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "esp_video_init failed: %s", esp_err_to_name(ret));
    return false;
  }
  
  ESP_LOGI(TAG, "✓ ESP-Video OK");
  return true;
}

// Config format V4L2
bool Tab5CameraV4L2::configure_format_v4l2_() {
  struct v4l2_format fmt = {};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 1280;
  fmt.fmt.pix.height = 720;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  
  if (ioctl(this->video_fd_, VIDIOC_S_FMT, &fmt) < 0) {
    ESP_LOGE(TAG, "VIDIOC_S_FMT failed: %d", errno);
    return false;
  }
  
  this->frame_buffer_size_ = fmt.fmt.pix.sizeimage;
  ESP_LOGI(TAG, "✓ Format: 1280x720 RGB565 (%u bytes)", this->frame_buffer_size_);
  return true;
}

// Setup buffers V4L2
bool Tab5CameraV4L2::setup_buffers_v4l2_() {
  struct v4l2_requestbuffers req = {};
  req.count = 3;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  
  if (ioctl(this->video_fd_, VIDIOC_REQBUFS, &req) < 0) {
    ESP_LOGE(TAG, "VIDIOC_REQBUFS failed");
    return false;
  }
  
  this->buffer_count_ = req.count;
  this->v4l2_buffers_ = (struct v4l2_buffer*)calloc(req.count, sizeof(struct v4l2_buffer));
  this->buffer_mappings_ = (BufferMapping*)calloc(req.count, sizeof(BufferMapping));
  
  for (uint32_t i = 0; i < req.count; i++) {
    struct v4l2_buffer buf = {};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    
    if (ioctl(this->video_fd_, VIDIOC_QUERYBUF, &buf) < 0) return false;
    
    void *start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                       MAP_SHARED, this->video_fd_, buf.m.offset);
    if (start == MAP_FAILED) return false;
    
    this->buffer_mappings_[i].start = start;
    this->buffer_mappings_[i].length = buf.length;
    
    if (ioctl(this->video_fd_, VIDIOC_QBUF, &buf) < 0) return false;
    this->v4l2_buffers_[i] = buf;
  }
  
  ESP_LOGI(TAG, "✓ %u buffers V4L2 mappés", req.count);
  return true;
}

// Start sensor
bool Tab5CameraV4L2::start_sensor_streaming_() {
  return this->sc202cs_write_reg_(SC202CS_REG_SLEEP_MODE, 0x01);  // Exit standby
}

// SETUP PRINCIPAL
void Tab5CameraV4L2::setup() {
  ESP_LOGI(TAG, "═══════════════════════════════════════════════════");
  ESP_LOGI(TAG, "🎥 Tab5 Camera + ESP-Video + SC202CS complet");
  ESP_LOGI(TAG, "═══════════════════════════════════════════════════");
  
  if (this->reset_pin_) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    delay(20);
  }
  
  delay(50);
  
  // 1. Init SC202CS avec TOUS les registres
  if (!this->init_sensor_sc202cs_()) {
    ESP_LOGE(TAG, "❌ Échec init SC202CS");
    this->mark_failed();
    return;
  }
  
  // 2. Configurer gain/exposition
  if (!this->configure_sensor_params_()) {
    ESP_LOGW(TAG, "⚠ Paramètres partiels");
  }
  
  // 3. Init ESP-Video
  if (!this->init_esp_video_()) {
    ESP_LOGE(TAG, "❌ Échec ESP-Video");
    this->mark_failed();
    return;
  }
  
  // 4. Ouvrir /dev/video0
  this->video_fd_ = open("/dev/video0", O_RDWR | O_NONBLOCK);
  if (this->video_fd_ < 0) {
    ESP_LOGE(TAG, "❌ Cannot open /dev/video0");
    this->mark_failed();
    return;
  }
  
  // 5. Config format
  if (!this->configure_format_v4l2_()) {
    ESP_LOGE(TAG, "❌ Format failed");
    this->mark_failed();
    return;
  }
  
  // 6. Setup buffers
  if (!this->setup_buffers_v4l2_()) {
    ESP_LOGE(TAG, "❌ Buffers failed");
    this->mark_failed();
    return;
  }
  
  // 7. Start sensor
  if (!this->start_sensor_streaming_()) {
    ESP_LOGW(TAG, "⚠ Sensor start failed");
  }
  
  this->initialized_ = true;
  
  ESP_LOGI(TAG, "═══════════════════════════════════════════════════");
  ESP_LOGI(TAG, "✅ Caméra prête");
  ESP_LOGI(TAG, "   Pipeline: SC202CS→CSI→ESP-Video→ISP→RGB565→V4L2");
  ESP_LOGI(TAG, "   Device: /dev/video0 (fd=%d)", this->video_fd_);
  ESP_LOGI(TAG, "═══════════════════════════════════════════════════");
}

bool Tab5CameraV4L2::start_streaming() {
  if (!this->initialized_ || this->streaming_) return this->streaming_;
  
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(this->video_fd_, VIDIOC_STREAMON, &type) < 0) {
    ESP_LOGE(TAG, "VIDIOC_STREAMON failed");
    return false;
  }
  
  this->streaming_ = true;
  this->total_frames_captured_ = 0;
  this->last_fps_report_time_ = millis();
  
  ESP_LOGI(TAG, "✅ Streaming @ 30 FPS");
  return true;
}

bool Tab5CameraV4L2::stop_streaming() {
  if (!this->streaming_) return true;
  
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  ioctl(this->video_fd_, VIDIOC_STREAMOFF, &type);
  this->streaming_ = false;
  
  return true;
}

bool Tab5CameraV4L2::capture_frame() {
  if (!this->streaming_) return false;
  
  struct v4l2_buffer buf = {};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  
  if (ioctl(this->video_fd_, VIDIOC_DQBUF, &buf) < 0) {
    if (errno == EAGAIN) return false;
    return false;
  }
  
  if (!(buf.flags & V4L2_BUF_FLAG_DONE) || buf.index >= this->buffer_count_) {
    ioctl(this->video_fd_, VIDIOC_QBUF, &buf);
    return false;
  }
  
  this->current_frame_buffer_ = (uint8_t*)this->buffer_mappings_[buf.index].start;
  ioctl(this->video_fd_, VIDIOC_QBUF, &buf);
  
  this->total_frames_captured_++;
  uint32_t now = millis();
  if (now - this->last_fps_report_time_ >= 5000) {
    float fps = (float)this->total_frames_captured_ / 
                ((now - this->last_fps_report_time_) / 1000.0f);
    ESP_LOGI(TAG, "📊 FPS: %.2f (%u frames)", fps, this->total_frames_captured_);
    this->total_frames_captured_ = 0;
    this->last_fps_report_time_ = now;
  }
  
  return true;
}

void Tab5CameraV4L2::loop() {}

void Tab5CameraV4L2::dump_config() {
  ESP_LOGCONFIG(TAG, "Tab5 Camera (ESP-Video):");
  ESP_LOGCONFIG(TAG, "  Résolution: 1280x720");
  ESP_LOGCONFIG(TAG, "  Format: RGB565");
  ESP_LOGCONFIG(TAG, "  Device: /dev/video0");
  ESP_LOGCONFIG(TAG, "  Streaming: %s", this->streaming_ ? "OUI" : "NON");
}

}  // namespace tab5_camera
}  // namespace esphome

#endif  // USE_ESP32_VARIANT_ESP32P4


