"""Script PlatformIO pour compiler ESP-Video"""

Import("env")
import os
import sys

# ============================================================================
# TROUVER LE RÉPERTOIRE ESP-VIDEO AUTOMATIQUEMENT
# ============================================================================

# Essayer d'obtenir le chemin du script via __file__, sinon fallback sur PROJECT_DIR
try:
    SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
except NameError:
    # __file__ n'est pas défini dans certains contextes ESPHome
    SCRIPT_DIR = os.path.join(env.get("PROJECT_DIR", ""), "components", "esp_video")

ESP_VIDEO_DIR = SCRIPT_DIR

print("=" * 80)
print("🎬 ESP-Video Build Script")
print(f"   Script location: {SCRIPT_DIR}")
print(f"   ESP-Video dir: {ESP_VIDEO_DIR}")
print("=" * 80)

# Vérifier que le répertoire src/ existe
src_dir = os.path.join(ESP_VIDEO_DIR, "src")
if not os.path.exists(src_dir):
    print(f"❌ ERROR: src/ not found in {ESP_VIDEO_DIR}")
    print("   This script must be in the esp_video/ directory")
    sys.exit(1)

# ============================================================================
# INCLUDES
# ============================================================================

include_paths = [
    os.path.join(ESP_VIDEO_DIR, "include"),
    os.path.join(ESP_VIDEO_DIR, "include", "linux"),
    os.path.join(ESP_VIDEO_DIR, "include", "sys"),
    os.path.join(ESP_VIDEO_DIR, "private_include"),
]

for path in include_paths:
    if os.path.exists(path):
        env.Append(CPPPATH=[path])
        print(f"✓ Include: {os.path.basename(path)}")
    else:
        print(f"⚠ Missing: {path}")

# ============================================================================
# SOURCES
# ============================================================================

base_sources = [
    "esp_video.c",
    "esp_video_buffer.c",
    "esp_video_init.c",
    "esp_video_ioctl.c",
    "esp_video_mman.c",
    "esp_video_vfs.c",
    "esp_video_cam.c",
    "esp_video_isp_pipeline.c",
]

device_sources = [
    "device/esp_video_csi_device.c",
    "device/esp_video_isp_device.c",
]

all_sources = []

for src in base_sources:
    src_path = os.path.join(src_dir, src)
    if os.path.exists(src_path):
        all_sources.append(src_path)
        print(f"✓ Source: {src}")
    else:
        print(f"⚠ Missing: {src}")

for src in device_sources:
    src_path = os.path.join(src_dir, src)
    if os.path.exists(src_path):
        all_sources.append(src_path)
        print(f"✓ Device: {src}")
    else:
        print(f"⚠ Missing: {src}")

# ============================================================================
# COMPILER
# ============================================================================

if all_sources:
    esp_video_lib = env.Library(
        target=os.path.join("$BUILD_DIR", "libesp_video"),
        source=all_sources
    )
    env.Append(LIBS=[esp_video_lib])
    print(f"✅ Compiled {len(all_sources)} sources into libesp_video.a")
else:
    print("❌ No sources found!")
    sys.exit(1)

# ============================================================================
# BUILD FLAGS
# ============================================================================

build_flags = [
    ("CONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE", "1"),
    ("CONFIG_ESP_VIDEO_ENABLE_ISP", "1"),
    ("CONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE", "1"),
    ("CONFIG_ESP_VIDEO_ENABLE_ISP_PIPELINE_CONTROLLER", "1"),
    ("CONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR", "1"),
    ("CONFIG_IDF_TARGET_ESP32P4", "1"),
]

for flag_name, flag_value in build_flags:
    env.Append(CPPDEFINES=[(flag_name, flag_value)])

print(f"✓ Added {len(build_flags)} build flags")
print("=" * 80)
print("✅ ESP-Video build configuration complete")
print("=" * 80)

