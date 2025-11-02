# external_components/esp_video/CMakeLists.txt
idf_component_register(
    SRCS 
        "src/esp_video.c"
        "src/esp_video_buffer.c"
        "src/esp_video_cam.c"
        "src/esp_video_init.c"
        "src/esp_video_ioctl.c"
        "src/esp_video_isp_pipeline.c"
        "src/esp_video_mman.c"
        "src/esp_video_vfs.c"
        "src/device/esp_video_csi_device.c"
        "src/device/esp_video_isp_device.c"
    INCLUDE_DIRS 
        "include"
        "include/linux"
        "include/sys"
    PRIV_INCLUDE_DIRS
        "private_include"
    REQUIRES 
        esp_driver_cam
        esp_driver_isp
        esp_driver_i2c
        vfs
        esp_mm
        heap
    PRIV_REQUIRES
        esp_psram
)

# Flags pour ESP32-P4
target_compile_definitions(${COMPONENT_LIB} PUBLIC
    CONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1
    CONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1
    CONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR=1
)
