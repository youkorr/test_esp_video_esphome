"""
esp_video_build.py
---------------------------------------------
Build script universel pour le composant ESP-Video (Espressif),
compatible avec ESPHome / PlatformIO / GitHub.

➡ Détection automatique :
   - esp_video/include
   - deps/include
   - private_include, linux/, sys/ sous-dossiers inclus.

Auteur : GPT-5 (OpenAI)
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
    print("[ESP-Video] ❌ Ce composant nécessite le framework ESP-IDF (pas Arduino).")
    print("👉 Ajoutez dans votre YAML : framework: type: esp-idf")
    sys.exit(1)

# ===============================================================
# 2️⃣ Détection automatique du dossier racine du composant
# ===============================================================

def find_component_root():
    """Trouve automatiquement le dossier contenant esp_video/include ou deps/include."""
    try:
        current_dir = os.path.dirname(os.path.abspath(__file__))
    except NameError:
        current_dir = os.getcwd()

    search_dir = current_dir
    for _ in range(8):  # Monter au maximum de 8 niveaux
        # Vérifier esp_video/include
        candidate1 = os.path.join(search_dir, "esp_video", "include")
        # Vérifier deps/include
        candidate2 = os.path.join(search_dir, "deps", "include")
        # Vérifier include directement
        candidate3 = os.path.join(search_dir, "include")

        if os.path.exists(candidate1):
            print(f"[ESP-Video] 📍 Détection: esp_video/include trouvé dans {search_dir}")
            return os.path.join(search_dir, "esp_video")
        if os.path.exists(candidate2):
            print(f"[ESP-Video] 📍 Détection: deps/include trouvé dans {search_dir}")
            return os.path.join(search_dir, "deps")
        if os.path.exists(candidate3) and "esp_video" in search_dir:
            print(f"[ESP-Video] 📍 Détection: include trouvé dans {search_dir}")
            return search_dir

        search_dir = os.path.dirname(search_dir)

    print("[ESP-Video] ⚠️ Aucun include détecté automatiquement, utilisation du dossier courant.")
    return current_dir


component_dir = find_component_root()
print(f"[ESP-Video] 📂 Dossier composant final : {component_dir}")

# ===============================================================
# 3️⃣ Ajout automatique des chemins d'inclusion
# ===============================================================

def add_include_if_exists(path):
    """Ajoute un dossier d'include si présent."""
    if os.path.exists(path):
        env.Append(CPPPATH=[path])
        print(f"[ESP-Video] ➕ Include ajouté : {path}")

# Ajouter le include principal et sous-dossiers
add_include_if_exists(os.path.join(component_dir, "include"))
add_include_if_exists(os.path.join(component_dir, "include", "linux"))
add_include_if_exists(os.path.join(component_dir, "include", "sys"))
add_include_if_exists(os.path.join(component_dir, "private_include"))

# Recherche récursive : si des sous-dossiers "esp_video" ou "deps" existent plus bas
for root, dirs, _ in os.walk(component_dir):
    for d in dirs:
        if d.lower() in ("esp_video", "deps"):
            possible_path = os.path.join(root, d, "include")
            add_include_if_exists(possible_path)
            add_include_if_exists(os.path.join(possible_path, "linux"))
            add_include_if_exists(os.path.join(possible_path, "sys"))
            add_include_if_exists(os.path.join(root, d, "private_include"))

# ===============================================================
# 4️⃣ Flags de compilation
# ===============================================================

build_flags = [
    "-DCONFIG_ESP_VIDEO_ENABLE_MIPI_CSI_VIDEO_DEVICE=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP_VIDEO_DEVICE=1",
    "-DCONFIG_ESP_VIDEO_ENABLE_ISP_PIPELINE_CONTROLLER=1",
    "-DCONFIG_ESP_VIDEO_USE_HEAP_ALLOCATOR=1",
]

for flag in build_flags:
    env.Append(CPPDEFINES=[flag.replace("-D", "", 1)])
    print(f"[ESP-Video] ✅ Flag ajouté : {flag}")

# ===============================================================
# 5️⃣ Informations et diagnostics
# ===============================================================

idf_path = env.get("IDF_PATH", "")
if not idf_path:
    print("[ESP-Video] ⚠️  IDF_PATH non défini — assurez-vous que l'environnement ESP-IDF est correct.")

project_dir = env.subst("$PROJECT_DIR")
if project_dir and os.path.exists(project_dir):
    try:
        rel_path = os.path.relpath(component_dir, project_dir)
        print(f"[ESP-Video] 🔗 Chemin relatif (depuis le projet) : {rel_path}")
    except Exception:
        print(f"[ESP-Video] (Info) Chemin absolu utilisé : {component_dir}")

# ===============================================================
# 6️⃣ Fin
# ===============================================================

print("[ESP-Video] ✅ Configuration du build terminée avec succès.")
print("[ESP-Video] -----------------------------------------------------\n")






