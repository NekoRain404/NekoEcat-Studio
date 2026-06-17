#pragma once

// Centralized language registry for NekoEcat Studio.
//
// The application uses inline bilingual strings via uiText(english, localized).
// This manager provides:
//   - A canonical list of supported languages (enum + display names)
//   - Lookup helpers for language-aware formatting (date, number, RTL detection)
//   - Future hook point for QTranslator / .ts file loading
//
// Usage:
//   const auto &lang = LanguageManager::instance();
//   if (lang.isCurrentLanguage(Language::ChineseSimplified)) { ... }

#include <QString>
#include <QStringList>
#include <QVector>

// Supported UI languages.  Values are stable — do not renumber.
enum class Language {
    English = 0,
    ChineseSimplified,
    // Future: Japanese, German, Korean, etc.
};

// Metadata for one supported language.
struct LanguageInfo {
    Language id;
    QString displayName;    // Native name shown in the settings combo ("English", "简体中文")
    QString localeCode;     // BCP-47 tag ("en", "zh-CN")
    bool rtl = false;       // Right-to-left script flag (future Arabic/Hebrew support)
};

class LanguageManager {
public:
    // Singleton access — the language list is compile-time constant.
    static LanguageManager &instance();

    // All registered languages (for populating combo boxes).
    const QVector<LanguageInfo> &languages() const;

    // Find language by display name (exact match).  Returns English if not found.
    Language fromDisplayName(const QString &name) const;

    // Find language by locale code.  Returns English if not found.
    Language fromLocaleCode(const QString &code) const;

    // Display name for a language enum.
    QString displayName(Language lang) const;

    // Convenience predicates.
    bool isCurrentLanguage(Language lang) const;
    Language currentLanguage() const;

    // Set the active language (called from SettingsDialog / applySettings).
    void setCurrentLanguage(Language lang);
    void setCurrentLanguage(const QString &displayName);

private:
    LanguageManager();
    QVector<LanguageInfo> languages_;
    Language current_ = Language::English;
};
