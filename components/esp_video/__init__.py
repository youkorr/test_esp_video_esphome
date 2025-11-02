"""Composant ESPHome pour ESP-Video d'Espressif"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.core import CORE
import os

CODEOWNERS = ["@espressif"]
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
    
    # Chemins d'include
    base = "external_components/esp_video"
    cg.add_build_flag(f"-I{base}/include")
    cg.add_build_flag(f"-I{base}/include/linux")
    cg.add_build_flag(f"-I{base}/include/sys")
    cg.add_build_flag(f"-I{base}/private_include")
    
    # Build flags
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_ISP=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_ISP_PIPELINE_CONTROLLER=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR=1")
    
    # Vérifier que le build script existe
    script = os.path.join(CORE.config_dir, base, "esp_video_build.py")
    if not os.path.exists(script):
        raise cv.Invalid(f"Script manquant: {script}")
