"""
esp_video_build.py — build script ESPHome
Ajoute automatiquement les includes esp_video + tab5_camera
"""

import os
import sys
from SCons.Script import Import

Import("env")

print("\n[ESP-Video] ⚙ Initialisation du build script (ESP-IDF uniquement)")

# ===============================================================
# Vérifier framework
# ===============================================================
framework = env.get("PIOFRAMEWORK", [])
if "espidf" not in framework:
    print("[ESP-Video] ❌ Ce composant nécessite le framework ESP-IDF.")
    sys.exit(1)

# ===============================================================
# Trouver le dossier esp_video
# ===============================================================
def find_component_root():
    try:
        current_dir = os.path.dirname(os.path.abspath(__file__))
    except NameError:
        current_dir = os.getcwd()

    for root, dirs, _ in os.walk("/data/external_components"):
        if "esp_video" in dirs:
            path = os.path.join(root, "esp_video")
            if os.path.exists(os.path.join(path, "include")):
                print(f"[ESP-Video] 📍 Include trouvé dans {path}/include")
                return path
    return current_dir

component_dir = find_component_root()
print(f"[ESP-Video] 📂 Dossier composant final : {component_dir}")

# ===============================================================
# Fonction utilitaire
# ===============================================================
def add_include_if_exists(path):
    if os.path.exists(path):
        env.Append(CPPPATH=[path])
        print(f"[ESP-Video] ➕ Include ajouté : {path}")

# ===============================================================
# Includes esp_video
# ===============================================================
add_include_if_exists(os.path.join(component_dir, "include"))
add_include_if_exists(os.path.join(component_dir, "include", "linux"))
add_include_if_exists(os.path.join(component_dir, "include", "sys"))
add_include_if_exists(os.path.join(component_dir, "private_include"))

# ===============================================================
# Includes deps/
# ===============================================================
deps_dir = os.path.join(component_dir, "deps")
if os.path.exists(deps_dir):
    print(f"[ESP-Video] 📦 Dossier deps détecté : {deps_dir}")
    add_include_if_exists(os.path.join(deps_dir, "include"))

# ===============================================================
# Redirection vers ton composant tab5_camera
# ===============================================================
project_dir = env.subst("$PROJECT_DIR")
tab5_cam_dir = os.path.join(project_dir, "src", "esphome", "components", "tab5_camera")

if os.path.exists(tab5_cam_dir):
    env.Append(CPPPATH=[tab5_cam_dir])
    print(f"[ESP-Video] 🎯 Redirection caméra vers : {tab5_cam_dir}")
else:
    # Variante si ESPHome monte les composants ailleurs
    alt_cam_dir = os.path.join("/data/build/tab5/src/esphome/components/tab5_camera")
    if os.path.exists(alt_cam_dir):
        env.Append(CPPPATH=[alt_cam_dir])
        print(f"[ESP-Video] 🎯 Redirection caméra (build) vers : {alt_cam_dir}")
    else:
        print("[ESP-Video] ⚠️ Composant tab5_camera introuvable — aucun redirect appliqué.")

# ===============================================================
# Flags de compilation
# ===============================================================
flags = [
    "-DCONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP_PIPELINE_CONTROLLER=1",
    "-DCONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR=1",
]

for flag in flags:
    env.Append(CPPDEFINES=[flag.replace("-D", "", 1)])
    print(f"[ESP-Video] ✅ Flag ajouté : {flag}")

print("[ESP-Video] ✅ Configuration du build terminée avec succès.\n")








