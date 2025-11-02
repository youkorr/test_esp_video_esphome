"""ESP-Video component for ESPHome with dependency stubs."""

import logging
import os
from pathlib import Path

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

_LOGGER = logging.getLogger(__name__)

CODEOWNERS = ["@youkorr"]
AUTO_LOAD = []
DEPENDENCIES = []

esp_video_ns = cg.esphome_ns.namespace("esp_video")

CONFIG_SCHEMA = cv.Schema({})


async def to_code(config):
    """Generate code for ESP-Video component."""
    
    # Trouver le dossier du composant
    component_dir = Path(__file__).parent
    deps_include = component_dir / "deps" / "include"
    
    # Vérifier que les stubs existent
    required_stubs = [
        "esp_cam_sensor.h",
        "esp_cam_sensor_xclk.h",
        "esp_sccb_i2c.h"
    ]
    
    _LOGGER.info("[ESP-Video] Vérification des stubs headers...")
    
    all_present = True
    for stub in required_stubs:
        stub_path = deps_include / stub
        if stub_path.exists():
            _LOGGER.info(f"[ESP-Video]   ✓ {stub}")
        else:
            _LOGGER.error(f"[ESP-Video]   ❌ {stub} MANQUANT!")
            all_present = False
    
    if not all_present:
        raise cv.Invalid(
            f"ESP-Video stubs manquants dans {deps_include}. "
            "Vérifiez que deps/include/ contient les 3 fichiers requis."
        )
    
    # Ajouter les includes AVANT la compilation
    _LOGGER.info(f"[ESP-Video] Ajout de deps/include au build: {deps_include}")
    
    # Ajouter au CPPPATH via platformio_options
    cg.add_platformio_option("build_flags", [
        f"-I{deps_include}",
        "-DCONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1",
        "-DCONFIG_ESP_VIDEO_ENABLE_ISP=1",
        "-DCONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1",
        "-DCONFIG_ESP_VIDEO_ENABLE_ISP_PIPELINE_CONTROLLER=1",
        "-DCONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR=1",
    ])
    
    _LOGGER.info("[ESP-Video] Configuration terminée")



