"""
Build script pour le composant ESP-Video (Espressif)
Ce script est automatiquement ajouté via to_code() dans le fichier esp_video/__init__.py.
Il configure les chemins d'inclusion et les options de compilation nécessaires.
"""

import os
import sys
from SCons.Script import Import

Import("env")

print("\n[ESP-Video] ⚙ Configuration du build script pour ESPHome (ESP-IDF uniquement)")

# =====================================================================
# 1️⃣ Vérification du framework
# =====================================================================

framework = env.get("PIOFRAMEWORK", [])
if "espidf" not in framework:
    print("[ESP-Video] ❌ ERREUR: Ce composant nécessite le framework ESP-IDF (pas Arduino).")
    print("👉 Ajoutez dans votre YAML ESPHome : framework: type: esp-idf")
    sys.exit(1)

# =====================================================================
# 2️⃣ Localisation du composant
# =====================================================================

# On tente de détecter le répertoire du composant depuis ce fichier
component_dir = os.path.dirname(os.path.abspath(__file__))
include_dir = os.path.join(component_dir, "include")

print(f"[ESP-Video] 📂 Répertoire du composant : {component_dir}")

# =====================================================================
# 3️⃣ Ajout des chemins d'inclusion
# =====================================================================

if os.path.exists(include_dir):
    env.Append(CPPPATH=[include_dir])
    print(f"[ESP-Video] ➕ Include path ajouté : {include_dir}")

# Ajouter également les sous-dossiers pour compatibilité avec ESP-IDF
for subdir in ["linux", "sys"]:
    sub_include = os.path.join(include_dir, subdir)
    if os.path.exists(sub_include):
        env.Append(CPPPATH=[sub_include])
        print(f"[ESP-Video] ➕ Include path ajouté : {sub_include}")

# Inclure les headers privés
private_include = os.path.join(component_dir, "private_include")
if os.path.exists(private_include):
    env.Append(CPPPATH=[private_include])
    print(f"[ESP-Video] ➕ Include path ajouté : {private_include}")

# =====================================================================
# 4️⃣ Définition des flags de compilation
# =====================================================================

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

# =====================================================================
# 5️⃣ Configuration additionnelle pour PlatformIO / ESPHome
# =====================================================================

# Si le projet utilise ESPHome, on ajoute un affichage clair du chemin du composant
project_dir = env.subst("$PROJECT_DIR")
if project_dir and os.path.exists(project_dir):
    rel_path = os.path.relpath(component_dir, project_dir)
    print(f"[ESP-Video] 🔗 Chemin relatif (depuis project_dir): {rel_path}")

# Vérifier les dépendances MIPI / ISP dans le SDK
idf_path = env.get("IDF_PATH", "")
if not idf_path:
    print("[ESP-Video] ⚠️  Avertissement: IDF_PATH non défini, vérifiez votre environnement ESP-IDF.")

# =====================================================================
# 6️⃣ Résumé
# =====================================================================

print("[ESP-Video] ✅ Configuration du build terminée.")
print("[ESP-Video] -----------------------------------------------------\n")



