#pragma once

// TranslationRegistry — runtime translation lookup for 8 languages.
//
// This registry maps English source strings to their localized equivalents.
// The uiText() function delegates here when the active language is not English.
//
// Architecture:
//   - English is the canonical key; all other languages are looked up by English text
//   - Untranslated strings fall back to the English key at runtime
//   - The registry is populated at construction from a static map (TranslationRegistry.cpp)
//
// Languages supported (indexed by Language enum - 1):
//   [0] zh-CN  简体中文
//   [1] ja     日本語
//   [2] de     Deutsch
//   [3] ko     한국어
//   [4] zh-TW  繁體中文
//   [5] fr     Français
//   [6] es     Español
//
// To add translations:
//   1. Add entries to TranslationRegistry.cpp init() method
//   2. For new strings, add uiText("English", "中文") calls as before
//   3. The registry automatically covers all languages for registered strings

#include "LanguageManager.h"

#include <QHash>
#include <QString>
#include <array>

class TranslationRegistry {
public:
    // Number of non-English languages (indexed by Language enum - 1).
    static constexpr int kLanguageCount = 7;

    static TranslationRegistry &instance();

    // Look up the translation for `english` in the given language.
    // Returns `english` if no translation exists or lang is English.
    QString translate(const QString &english, Language lang) const;

private:
    TranslationRegistry();
    void init();

    // Maps English source text → array of [zh-CN, ja, de, ko, zh-TW, fr, es].
    QHash<QString, std::array<QString, kLanguageCount>> map_;
};
