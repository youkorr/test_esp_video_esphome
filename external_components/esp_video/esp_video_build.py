"""Script PlatformIO pour compiler ESP-Video"""

Import("env")
import os

ESP_VIDEO_DIR = os.path.join(env.subst("$PROJECT_DIR"), "external_components", "esp_video")

print("=" * 60)
print("🎬 ESP-Video Build Script")
print(f"   Directory: {ESP_VIDEO_DIR}")

# Includes
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

# Sources
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
src_dir = os.path.join(ESP_VIDEO_DIR, "src")

for src in base_sources + device_sources:
    src_path = os.path.join(src_dir, src)
    if os.path.exists(src_path):
        all_sources.append(src_path)

if all_sources:
    esp_video_lib = env.Library(
        target=os.path.join("$BUILD_DIR", "esp_video"),
        source=all_sources
    )
    env.Append(LIBS=[esp_video_lib])
    print(f"✓ Compiled {len(all_sources)} sources")
else:
    print("❌ No sources found!")

# Build flags
flags = [
    "-DCONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP_PIPELINE_CONTROLLER=1",
    "-DCONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR=1",
]

for flag in flags:
    env.Append(CPPDEFINES=[flag.replace("-D", "").split("=")[0] + "=" + flag.split("=")[1]])

print(f"✓ Added {len(flags)} flags")
print("=" * 60)
