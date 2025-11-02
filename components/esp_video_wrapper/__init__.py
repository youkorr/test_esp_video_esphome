import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@yourusername"]
DEPENDENCIES = ["esp32"]

esp_video_wrapper_ns = cg.esphome_ns.namespace("esp_video_wrapper")
ESPVideoWrapper = esp_video_wrapper_ns.class_("ESPVideoWrapper", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ESPVideoWrapper),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Ajouter les chemins d'inclusion pour esp-video
    cg.add_build_flag("-Icomponents/esp_video_wrapper/esp_video/include")
    cg.add_build_flag("-Icomponents/esp_video_wrapper/esp_video/private_include")
    
    # Flags de compilation pour esp-video
    cg.add_build_flag("-DESP_VIDEO_VER_MAJOR=1")
    cg.add_build_flag("-DESP_VIDEO_VER_MINOR=0")
    cg.add_build_flag("-DESP_VIDEO_VER_PATCH=0")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_ISP=1")
    cg.add_build_flag("-DCONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1")
