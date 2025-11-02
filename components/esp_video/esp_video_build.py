"""
Build script pour le composant ESP-Video (Espressif)
Compatible avec ESPHome / PlatformIO même sans __file__ défini.
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
# 2️⃣ Détection du dossier du composant
# ===============================================================

# Certains contextes ESPHome n’ont pas __file__
try:
    component_dir = os.path.dirname(os.path.abspath(__file__))
except NameError:
    # Fallback: essayer de le retrouver à partir du nom du script dans Extra Scripts
    script_paths = env.GetExtraScripts("post")
    if script_paths:
        component_dir = os.path.dirname(os.path.abspath(script_paths[-1]))
    else:
        component_dir = os.getcwd()

print(f"[ESP-Video] 📂 Répertoire du composant : {component_dir}")

include_dir = os.path.join(component_dir, "include")

# ===============================================================
# 3️⃣ Ajout des chemins d'inclusion
# ===============================================================

if os.path.exists(include_dir):
    env.Append(CPPPATH=[include_dir])
    print(f"[ESP-Video] ➕ Include path ajouté : {include_dir}")

# Ajouter aussi les sous-répertoires si présents
for subdir in ["linux", "sys"]:
    sub_include = os.path.join(include_dir, subdir)
    if os.path.exists(sub_include):
        env.Append(CPPPATH=[sub_include])
        print(f"[ESP-Video] ➕ Include path ajouté : {sub_include}")

# Dossier private_include
private_include = os.path.join(component_dir, "private_include")
if os.path.exists(private_include):
    env.Append(CPPPATH=[private_include])
    print(f"[ESP-Video] ➕ Include path ajouté : {private_include}")

# ===============================================================
# 4️⃣ Définition des flags de compilation
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
# 5️⃣ Vérification environnement ESP-IDF
# ===============================================================

idf_path = env.get("IDF_PATH", "")
if not idf_path:
    print("[ESP-Video] ⚠️  Avertissement: IDF_PATH non défini (vérifiez votre installation ESP-IDF).")

# ===============================================================
# 6️⃣ Informations supplémentaires
# ===============================================================

project_dir = env.subst("$PROJECT_DIR")
if project_dir and os.path.exists(project_dir):
    try:
        rel_path = os.path.relpath(component_dir, project_dir)
        print(f"[ESP-Video] 🔗 Chemin relatif (depuis project_dir): {rel_path}")
    except Exception:
        print(f"[ESP-Video] (Info) Chemin absolu utilisé : {component_dir}")

# ===============================================================
# 7️⃣ Fin du script
# ===============================================================

print("[ESP-Video] ✅ Configuration du build terminée avec succès.")
print("[ESP-Video] -----------------------------------------------------\n")




