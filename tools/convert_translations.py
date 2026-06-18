#!/usr/bin/env python3
"""Convert TranslationRegistry.cpp entries to Qt .ts XML files.

Reads apps/ecat-studio/infra/TranslationRegistry.cpp and generates
one .ts file per target language under translations/.

Language order in the cpp array: [0]=zh-CN [1]=ja [2]=de [3]=ko [4]=zh-TW [5]=fr [6]=es
"""

import re
import os
import xml.etree.ElementTree as ET
from xml.dom import minidom

CPP_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..",
                        "apps", "ecat-studio", "infra", "TranslationRegistry.cpp")
OUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "translations")

# Map array index -> (language code for .ts, filename suffix)
LANGUAGES = [
    (0, "zh_CN",  "zh"),
    (1, "ja",     "ja"),
    (2, "de",     "de"),
    (3, "ko",     "ko"),
    (4, "zh_TW",  "zh_TW"),
    (5, "fr",     "fr"),
    (6, "es",     "es"),
]


def parse_entries(cpp_path):
    """Parse all map_ entries from TranslationRegistry.cpp.
    Returns list of (english_key, [zh, ja, de, ko, zh-tw, fr, es]).
    """
    with open(cpp_path, "r", encoding="utf-8") as f:
        content = f.read()

    pattern = re.compile(
        r'map_\[QStringLiteral\("((?:[^"\\]|\\.)*)"\)\]\s*=\s*\{([^}]+)\};',
        re.DOTALL,
    )
    literal_pattern = re.compile(r'QStringLiteral\("((?:[^"\\]|\\.)*)"\)')

    entries = []
    for m in pattern.finditer(content):
        key = m.group(1)
        values_block = m.group(2)
        literals = literal_pattern.findall(values_block)
        if len(literals) >= 7:
            entries.append((key, literals[:7]))
        else:
            padded = literals + [""] * (7 - len(literals))
            entries.append((key, padded))

    return entries


def build_ts_xml(entries, lang_code, lang_index):
    """Build a Qt .ts XML string for the given language."""
    ts = ET.Element("TS")
    ts.set("version", "2.1")
    ts.set("language", lang_code)

    context = ET.SubElement(ts, "context")
    name_el = ET.SubElement(context, "name")
    name_el.text = "MainWindow"

    for english, translations in entries:
        msg = ET.SubElement(context, "message")
        src = ET.SubElement(msg, "source")
        src.text = english
        trans_el = ET.SubElement(msg, "translation")
        translated = translations[lang_index] if lang_index < len(translations) else ""
        trans_el.text = translated
        if not translated or translated == english:
            trans_el.set("type", "unfinished")

    rough = ET.tostring(ts, encoding="unicode")
    parsed = minidom.parseString(rough)
    lines = parsed.toprettyxml(indent="    ", encoding="utf-8").decode("utf-8")
    if lines.startswith("<?xml"):
        lines = lines[lines.index("?>") + 2:].lstrip("\n")
    return '<?xml version="1.0" encoding="utf-8"?>\n<!DOCTYPE TS>\n' + lines


def main():
    cpp_path = os.path.normpath(CPP_PATH)
    out_dir = os.path.normpath(OUT_DIR)
    os.makedirs(out_dir, exist_ok=True)

    entries = parse_entries(cpp_path)
    print(f"Parsed {len(entries)} translation entries from {cpp_path}")

    for idx, ts_lang, suffix in LANGUAGES:
        xml_str = build_ts_xml(entries, ts_lang, idx)
        filename = f"nekoecat_{suffix}.ts"
        filepath = os.path.join(out_dir, filename)
        with open(filepath, "w", encoding="utf-8") as f:
            f.write(xml_str)
        print(f"  Written {filepath} ({len(entries)} messages)")

    print("Done.")


if __name__ == "__main__":
    main()
