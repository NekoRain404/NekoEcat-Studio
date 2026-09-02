#pragma once

// LanguageManager — centralized language registry for NekoEcat Studio.
//
// Supports 8 languages for global EtherCAT development teams:
//   English, 简体中文, 日本語, Deutsch, 한국어, 繁體中文, Français, Español
//
// The application uses inline bilingual strings via uiText(english, localized).
// This manager provides:
//   - A canonical list of supported languages (enum + display names)
//   - Lookup helpers for language-aware formatting
//   - Singleton access for the language list (compile-time constant)
//
// Usage:
//   const auto &lang = LanguageManager::instance();
//   if (lang.isCurrentLanguage(Language::Japanese)) { ... }

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

// Supported UI languages.  Values are stable — do not renumber.
enum class Language {
    English = 0,
    ChineseSimplified,  // 简体中文
    Japanese,           // 日本語
    German,             // Deutsch
    Korean,             // 한국어
    ChineseTraditional, // 繁體中文
    French,             // Français
    Spanish,            // Español
};

// Metadata for one supported language.
struct LanguageInfo {
    Language id;
    QString displayName; // Native name shown in the settings combo
    QString localeCode;  // BCP-47 tag
    bool rtl = false;    // Right-to-left script flag (future Arabic/Hebrew)
};

class LanguageManager : public QObject {
    Q_OBJECT
public:
    // Singleton access — the language list is compile-time constant.
    static LanguageManager& instance();

    // All registered languages (for populating combo boxes).
    const QVector<LanguageInfo>& languages() const;

    // Find language by display name (exact match).  Returns English if not found.
    Language fromDisplayName(const QString& name) const;

    // Find language by locale code.  Returns English if not found.
    Language fromLocaleCode(const QString& code) const;

    // Qt .ts/.qm basename suffix for a language (e.g. "zh", "zh_TW").  These
    // follow the translation-file naming under translations/, which differs
    // from the BCP-47 localeCode (e.g. "zh-CN").
    QString translationFileSuffix(Language lang) const;

    // Display name for a language enum.
    QString displayName(Language lang) const;

    // Convenience predicates.
    bool isCurrentLanguage(Language lang) const;
    Language currentLanguage() const;

    // Set the active language (called from SettingsDialog / applySettings).
    void setCurrentLanguage(Language lang);
    void setCurrentLanguage(const QString& displayName);

signals:
    void languageChanged(Language lang);

private:
    explicit LanguageManager(QObject* parent = nullptr);
    QVector<LanguageInfo> languages_;
    Language current_ = Language::English;
};
