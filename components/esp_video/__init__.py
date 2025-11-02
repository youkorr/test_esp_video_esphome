"""Composant ESPHome pour ESP-Video d'Espressif"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.core import CORE
import os

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ["esp32"]

esp_video_ns = cg.esphome_ns.namespace("esp_video")
ESPVideoComponent = esp_video_ns.class_("ESPVideoComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ESPVideoComponent),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    if CORE.using_arduino:
        raise cv.Invalid("esp_video nécessite framework: type: esp-idf")
    
    # ========================================================================
    # TROUVER LE CHEMIN DU COMPOSANT (même depuis GitHub)
    # ========================================================================
    
    component_dir = os.path.dirname(os.path.abspath(__file__))
    
    try:
        rel_path = os.path.relpath(component_dir, CORE.config_dir)
    except ValueError:
        rel_path = component_dir
    
    print(f"ESP-Video component directory: {component_dir}")
    print(f"Relative path: {rel_path}")
    
    # ========================================================================
    # INCLUDES
    # ========================================================================
    
    cg.add_build_flag(f"-I{component_dir}/include")
    cg.add_build_flag(f"-I{component_dir}/include/linux")
    cg.add_build_flag(f"-I{component_dir}/include/sys")
    cg.add_build_flag(f"-I{component_dir}/private_include")
    
    # ========================================================================
    # BUILD FLAGS
    # ========================================================================
    
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_ISP=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_ISP_PIPELINE_CONTROLLER=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR=1")
    
    # ========================================================================
    # AJOUTER LE BUILD SCRIPT AUTOMATIQUEMENT (CHEMIN ABSOLU)
    # ========================================================================
    
    build_script = os.path.join(component_dir, "esp_video_build.py")
    
    if os.path.exists(build_script):
        cg.add_platformio_option("extra_scripts", [f"post:{build_script}"])
        print(f"✓ Added build script: {build_script}")
    else:
        raise cv.Invalid(f"Build script not found: {build_script}")
    
    # ========================================================================
    # LOG RUNTIME (VISIBLE DANS ESPHOME LOGS)
    # ========================================================================
    
    # Ce message apparaîtra au démarrage du firmware (pas seulement à la compilation)
    cg.add_global(cg.RawExpression('ESP_LOGI("esp_video", "🎬 ESP-Video component loaded successfully");'))


