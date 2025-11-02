"""
esp_video_build.py — Script FORCANT la création des stubs
"""
import os
import sys
from SCons.Script import Import

Import("env")

print("\n[ESP-Video] ⚙ Initialisation du build script")

framework = env.get("PIOFRAMEWORK", [])
if "espidf" not in framework:
    print("[ESP-Video] ❌ ESP-IDF requis")
    sys.exit(1)

def find_component_root():
    try:
        current_dir = os.path.dirname(os.path.abspath(__file__))
        return current_dir
    except NameError:
        pass
    
    for root, dirs, _ in os.walk("/data/external_components"):
        if "esp_video" in dirs:
            path = os.path.join(root, "esp_video")
            if os.path.exists(os.path.join(path, "include")):
                return path
    return os.getcwd()

component_dir = find_component_root()
print(f"[ESP-Video] 📂 Composant: {component_dir}")

def add_include(path):
    if os.path.exists(path):
        env.Append(CPPPATH=[path])
        print(f"[ESP-Video] ➕ Include: {os.path.basename(path)}")

# Includes de base
add_include(os.path.join(component_dir, "include"))
add_include(os.path.join(component_dir, "include", "linux"))
add_include(os.path.join(component_dir, "include", "sys"))
add_include(os.path.join(component_dir, "private_include"))

# ===============================================================
# CRÉER LES STUBS - TOUJOURS
# ===============================================================
deps_dir = os.path.join(component_dir, "deps", "include")

print(f"[ESP-Video] 🔧 Création forcée des stubs dans: {deps_dir}")

try:
    os.makedirs(deps_dir, exist_ok=True)
    
    # esp_cam_sensor.h - TOUJOURS recréer
    stub1 = os.path.join(deps_dir, "esp_cam_sensor.h")
    with open(stub1, 'w') as f:
        f.write('''#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { ESP_CAM_SENSOR_MIPI_CSI = 0, ESP_CAM_SENSOR_DVP = 1 } esp_cam_sensor_port_t;
typedef struct { void *sccb_handle; int reset_pin; int pwdn_pin; int xclk_pin; uint32_t xclk_freq_hz; esp_cam_sensor_port_t sensor_port; } esp_cam_sensor_config_t;
typedef struct { uint16_t pid; } esp_cam_sensor_id_t;
typedef struct { uint16_t width; uint16_t height; uint32_t format; } esp_cam_sensor_format_t;
typedef struct esp_cam_sensor_device_t esp_cam_sensor_device_t;
typedef struct { esp_err_t (*query_para_desc)(esp_cam_sensor_device_t*, void*); esp_err_t (*get_para_value)(esp_cam_sensor_device_t*, uint32_t, void*, size_t); esp_err_t (*set_para_value)(esp_cam_sensor_device_t*, uint32_t, const void*, size_t); esp_err_t (*query_support_formats)(esp_cam_sensor_device_t*, void*); esp_err_t (*query_support_capability)(esp_cam_sensor_device_t*, void*); esp_err_t (*set_format)(esp_cam_sensor_device_t*, const void*); esp_err_t (*get_format)(esp_cam_sensor_device_t*, void*); esp_err_t (*priv_ioctl)(esp_cam_sensor_device_t*, uint32_t, void*); esp_err_t (*del)(esp_cam_sensor_device_t*); } esp_cam_sensor_ops_t;
struct esp_cam_sensor_device_t { char *name; void *sccb_handle; int xclk_pin; int reset_pin; int pwdn_pin; esp_cam_sensor_port_t sensor_port; esp_cam_sensor_id_t id; const esp_cam_sensor_ops_t *ops; void *priv; bool stream_status; };
esp_err_t esp_cam_sensor_ioctl(esp_cam_sensor_device_t *dev, uint32_t cmd, void *arg);
#ifdef __cplusplus
}
#endif
''')
    print(f"[ESP-Video]   ✓ {stub1}")
    
    # esp_cam_sensor_xclk.h
    stub2 = os.path.join(deps_dir, "esp_cam_sensor_xclk.h")
    with open(stub2, 'w') as f:
        f.write('''#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { gpio_num_t xclk_pin; uint32_t xclk_freq_hz; } esp_cam_sensor_xclk_config_t;
esp_err_t esp_cam_sensor_xclk_init(esp_cam_sensor_xclk_config_t *config);
esp_err_t esp_cam_sensor_xclk_deinit(void);
#ifdef __cplusplus
}
#endif
''')
    print(f"[ESP-Video]   ✓ {stub2}")
    
    # esp_sccb_i2c.h
    stub3 = os.path.join(deps_dir, "esp_sccb_i2c.h")
    with open(stub3, 'w') as f:
        f.write('''#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct esp_sccb_io_t *esp_sccb_io_handle_t;
typedef enum { I2C_ADDR_BIT_LEN_7 = 0, I2C_ADDR_BIT_LEN_10 } i2c_addr_bit_len_t;
typedef struct { uint8_t device_address; i2c_addr_bit_len_t dev_addr_length; uint32_t scl_speed_hz; uint8_t addr_bits_width; uint8_t val_bits_width; } sccb_i2c_config_t;
esp_err_t sccb_new_i2c_io(i2c_master_bus_handle_t bus, const sccb_i2c_config_t *cfg, esp_sccb_io_handle_t *ret);
esp_err_t esp_sccb_transmit_reg_a16v8(esp_sccb_io_handle_t h, uint16_t reg, uint8_t data);
esp_err_t esp_sccb_transmit_receive_reg_a16v8(esp_sccb_io_handle_t h, uint16_t reg, uint8_t *data);
esp_err_t esp_sccb_del_i2c_io(esp_sccb_io_handle_t h);
#ifdef __cplusplus
}
#endif
''')
    print(f"[ESP-Video]   ✓ {stub3}")
    
    # Ajouter au CPPPATH
    env.Append(CPPPATH=[deps_dir])
    print(f"[ESP-Video] ➕ Include deps ajouté: {deps_dir}")
    
except Exception as e:
    print(f"[ESP-Video] ⚠️ Erreur création stubs: {e}")
    import traceback
    traceback.print_exc()

# Tab5 camera
project_dir = env.subst("$PROJECT_DIR")
for path in [
    os.path.join(project_dir, "src/esphome/components/tab5_camera"),
    "/data/build/tab5/src/esphome/components/tab5_camera"
]:
    if os.path.exists(path):
        env.Append(CPPPATH=[path])
        print(f"[ESP-Video] 🎯 tab5_camera: {path}")
        break

# Flags
for flag in [
    "CONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1",
    "CONFIG_ESP_VIDEO_ENABLE_ISP=1",
    "CONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1",
    "CONFIG_ESP_VIDEO_ENABLE_ISP_PIPELINE_CONTROLLER=1",
    "CONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR=1",
]:
    env.Append(CPPDEFINES=[flag.replace("-D", "")])

print("[ESP-Video] ✅ Configuration terminée\n")








