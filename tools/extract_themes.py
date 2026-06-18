#!/usr/bin/env python3
"""Extract QSS theme strings from MainWindowTheme.cpp into individual .qss files."""

import re
import os

THEME_CPP = "apps/ecat-studio/workspaces/MainWindowTheme.cpp"
THEMES_DIR = "apps/ecat-studio/themes"
QRC_FILE = os.path.join(THEMES_DIR, "themes.qrc")

# Theme name → filename mapping (lowercase, no spaces)
THEME_FILE_MAP = {
    "Dark":      "dark.qss",
    "Light":     "light.qss",
    "Nord":      "nord.qss",
    "Catppuccin":"catppuccin.qss",
    "Dracula":   "dracula.qss",
    "Solarized": "solarized.qss",
    "Gruvbox":   "gruvbox.qss",
    "TokyoNight":"tokyonight.qss",
    "OneDark":   "onedark.qss",
    "Monokai":   "monokai.qss",
    "Cyberpunk": "cyberpunk.qss",
}

def extract_themes():
    with open(THEME_CPP, "r", encoding="utf-8") as f:
        content = f.read()

    # Find each function body: qss<Name>() { return QStringLiteral(R"QSS( ... )QSS"); }
    pattern = re.compile(
        r'QString\s+qss(\w+)\s*\(\s*\)\s*\{[^}]*?R"QSS\(\s*\n(.*?)\s*\)QSS"',
        re.DOTALL
    )

    matches = pattern.findall(content)
    print(f"Found {len(matches)} theme functions")

    os.makedirs(THEMES_DIR, exist_ok=True)

    theme_files = []
    for name, qss_body in matches:
        if name not in THEME_FILE_MAP:
            print(f"  WARNING: unknown theme function '{name}', skipping")
            continue
        fname = THEME_FILE_MAP[name]
        fpath = os.path.join(THEMES_DIR, fname)
        with open(fpath, "w", encoding="utf-8") as f:
            f.write(qss_body.rstrip() + "\n")
        line_count = qss_body.count("\n") + 1
        print(f"  {name}: {line_count} lines -> {fname}")
        theme_files.append(fname)

    # Generate themes.qrc
    qrc_lines = ['<RCC>', '  <qresource prefix="/themes">']
    for fname in sorted(theme_files):
        qrc_lines.append(f'    <file alias="{fname}">{fname}</file>')
    qrc_lines += ['  </qresource>', '</RCC>', '']
    with open(QRC_FILE, "w", encoding="utf-8") as f:
        f.write("\n".join(qrc_lines))
    print(f"\nWrote {QRC_FILE} with {len(theme_files)} entries")

if __name__ == "__main__":
    extract_themes()
