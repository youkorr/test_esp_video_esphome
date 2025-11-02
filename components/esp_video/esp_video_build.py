"""
esp_video_build.py
--------------------------------------------------
Build script universel pour ESP-Video + dépendances caméra (esp_cam).
Compatible ESPHome / PlatformIO / GitHub.
"""

import os
import sys
from SCons.Script import Import

Import("env")

print("\n[ESP-Video] ⚙ Initialisation du build script (ESP-IDF uniquement)")

# ===============================================================
# 1️⃣ Vérification du framework
# ===============================================================

framework = env.get("PIOFRAMEWORK", [])
if "espidf" not in framework:
    print("[ESP-Video] ❌ Ce composant nécessite le framework ESP-IDF.")
    sys.exit(1)

# ===============================================================
# 2️⃣ Détection automatique du dossier racine du composant
# ===============================================================

def find_component_root():
    """Trouve automatiquement le dossier racine du composant esp_video."""
    try:
        current_dir = os.path.dirname(os.path.abspath(__file__))
    except NameError:
        current_dir = os.getcwd()

    search_roots = [
        current_dir,
        os.path.dirname(current_dir),
        os.path.dirname(os.path.dirname(current_dir)),
        "/data/external_components",
        "/data/build",
        "/config/esphome",
    ]

    for root in search_roots:
        if not os.path.exists(root):
            continue
        for dirpath, dirnames, _ in os.walk(root):
            if "esp_video" in dirnames:
                path = os.path.join(dirpath, "esp_video")
                if os.path.exists(os.path.join(path, "include")):
                    print(f"[ESP-Video] 📍 Include trouvé dans {path}/include")
                    return path
    print("[ESP-Video] ⚠️ Aucun include détecté automatiquement, utilisation du dossier courant.")
    return current_dir


component_dir = find_component_root()
print(f"[ESP-Video] 📂 Dossier composant final : {component_dir}")

# ===============================================================
# 3️⃣ Ajout automatique des chemins d'inclusion
# ===============================================================

def add_include_if_exists(path):
    if os.path.exists(path):
        env.Append(CPPPATH=[path])
        print(f"[ESP-Video] ➕ Include ajouté : {path}")

# ESP-Video includes
add_include_if_exists(os.path.join(component_dir, "include"))
add_include_if_exists(os.path.join(component_dir, "include", "linux"))
add_include_if_exists(os.path.join(component_dir, "include", "sys"))
add_include_if_exists(os.path.join(component_dir, "private_include"))

# ===============================================================
# 4️⃣ Détection et ajout des dossiers deps/
# ===============================================================

deps_dir = os.path.join(component_dir, "deps")
if os.path.exists(deps_dir):
    print(f"[ESP-Video] 📦 Dossier deps détecté : {deps_dir}")
    # Inclure deps/include
    add_include_if_exists(os.path.join(deps_dir, "include"))
    add_include_if_exists(os.path.join(deps_dir, "private_include"))

    # Recherche spécifique pour deps/esp_cam
    esp_cam_dir = os.path.join(deps_dir, "esp_cam")
    if os.path.exists(esp_cam_dir):
        print(f"[ESP-Video] 🎥 Dépendance caméra détectée : {esp_cam_dir}")
        add_include_if_exists(os.path.join(esp_cam_dir, "include"))
        add_include_if_exists(os.path.join(esp_cam_dir, "include", "driver"))
        add_include_if_exists(os.path.join(esp_cam_dir, "private_include"))
    else:
        print("[ESP-Video] 🔍 Aucune dépendance caméra (esp_cam) trouvée dans deps/")
else:
    print("[ESP-Video] ⚠️ Aucun dossier deps/ détecté à côté de esp_video/")

# ===============================================================
# 5️⃣ Flags de compilation
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

# ===============================================================
# 6️⃣ Informations environnementales
# ===============================================================

idf_path = env.get("IDF_PATH", "")
if not idf_path:
    print("[ESP-Video] ⚠️  IDF_PATH non défini — vérifiez l'environnement ESP-IDF.")

project_dir = env.subst("$PROJECT_DIR")
if project_dir and os.path.exists(project_dir):
    try:
        rel_path = os.path.relpath(component_dir, project_dir)
        print(f"[ESP-Video] 🔗 Chemin relatif (depuis projet): {rel_path}")
    except Exception:
        print(f"[ESP-Video] (Info) Chemin absolu utilisé: {component_dir}")

# ===============================================================
# 7️⃣ Fin
# ===============================================================

print("[ESP-Video] ✅ Configuration du build terminée avec succès.")
print("[ESP-Video] -----------------------------------------------------\n")







