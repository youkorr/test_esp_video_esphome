"""
esp_video_build.py
---------------------------------------------
Build script universel ESP-Video
Compatible ESPHome / PlatformIO
Détection automatique : esp_video/include OU deps/include
y compris dans /data/external_components/...
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
# 2️⃣ Détection avancée du dossier racine du composant
# ===============================================================

def find_component_root():
    """Trouve automatiquement esp_video/include ou deps/include, même dans external_components."""
    try:
        current_dir = os.path.dirname(os.path.abspath(__file__))
    except NameError:
        current_dir = os.getcwd()

    search_roots = [
        current_dir,
        os.path.dirname(current_dir),
        os.path.dirname(os.path.dirname(current_dir)),
        "/data/external_components",
        "/config/esphome",
        "/data/build",
    ]

    for root in search_roots:
        if not os.path.exists(root):
            continue

        for dirpath, dirnames, _ in os.walk(root):
            for d in dirnames:
                if d.lower() in ("esp_video", "deps"):
                    include_path = os.path.join(dirpath, d, "include")
                    if os.path.exists(include_path):
                        print(f"[ESP-Video] 📍 Include trouvé dans {include_path}")
                        return os.path.join(dirpath, d)
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

# Inclure tous les répertoires pertinents
add_include_if_exists(os.path.join(component_dir, "include"))
add_include_if_exists(os.path.join(component_dir, "include", "linux"))
add_include_if_exists(os.path.join(component_dir, "include", "sys"))
add_include_if_exists(os.path.join(component_dir, "private_include"))

# ===============================================================
# 4️⃣ Flags de compilation
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
# 5️⃣ Informations environnementales
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
# 6️⃣ Fin
# ===============================================================

print("[ESP-Video] ✅ Configuration du build terminée avec succès.")
print("[ESP-Video] -----------------------------------------------------\n")







