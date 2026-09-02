// LanguageManager implementation — centralized language registry for 8 languages.
#include "LanguageManager.h"

LanguageManager& LanguageManager::instance() {
    static LanguageManager mgr;
    return mgr;
}

LanguageManager::LanguageManager(QObject* parent) : QObject(parent) {
    languages_ = {
        {Language::English, QStringLiteral("English"), QStringLiteral("en"), false},
        {Language::ChineseSimplified, QStringLiteral("简体中文"), QStringLiteral("zh-CN"), false},
        {Language::Japanese, QStringLiteral("日本語"), QStringLiteral("ja"), false},
        {Language::German, QStringLiteral("Deutsch"), QStringLiteral("de"), false},
        {Language::Korean, QStringLiteral("한국어"), QStringLiteral("ko"), false},
        {Language::ChineseTraditional, QStringLiteral("繁體中文"), QStringLiteral("zh-TW"), false},
        {Language::French, QStringLiteral("Français"), QStringLiteral("fr"), false},
        {Language::Spanish, QStringLiteral("Español"), QStringLiteral("es"), false},
    };
}

const QVector<LanguageInfo>& LanguageManager::languages() const {
    return languages_;
}

// Looks up a language enum by its localized display name
Language LanguageManager::fromDisplayName(const QString& name) const {
    // Iterate over collection
    for (const auto& info : languages_) {
        if (info.displayName == name) {
            return info.id;
        }
    }
    return Language::English;
}

// Looks up a language enum by its locale code (e.g., "zh-CN")
Language LanguageManager::fromLocaleCode(const QString& code) const {
    // Iterate over collection
    for (const auto& info : languages_) {
        if (info.localeCode == code) {
            return info.id;
        }
    }
    return Language::English;
}

// Returns the localized display name for a language
QString LanguageManager::displayName(Language lang) const {
    // Iterate over collection
    for (const auto& info : languages_) {
        if (info.id == lang) {
            return info.displayName;
        }
    }
    return QStringLiteral("English");
}

// Returns the Qt translation-file basename suffix (matches the .ts/.qm files
// under translations/, e.g. nekoecat_zh.ts / nekoecat_zh_TW.ts).  This is
// distinct from the BCP-47 localeCode ("zh-CN" vs "zh").
QString LanguageManager::translationFileSuffix(Language lang) const {
    switch (lang) {
        case Language::English:
            return QStringLiteral("en");
        case Language::ChineseSimplified:
            return QStringLiteral("zh");
        case Language::Japanese:
            return QStringLiteral("ja");
        case Language::German:
            return QStringLiteral("de");
        case Language::Korean:
            return QStringLiteral("ko");
        case Language::ChineseTraditional:
            return QStringLiteral("zh_TW");
        case Language::French:
            return QStringLiteral("fr");
        case Language::Spanish:
            return QStringLiteral("es");
    }
    return QStringLiteral("en");
}

// Checks if the given language is the currently active one
bool LanguageManager::isCurrentLanguage(Language lang) const {
    return current_ == lang;
}

// Returns the currently active language enum
Language LanguageManager::currentLanguage() const {
    return current_;
}

// Switches the active language and emits a change signal
void LanguageManager::setCurrentLanguage(Language lang) {
    if (current_ != lang) {
        current_ = lang;
        emit languageChanged(lang);
    }
}

// Switches the active language and emits a change signal
void LanguageManager::setCurrentLanguage(const QString& displayName) {
    setCurrentLanguage(fromDisplayName(displayName));
}
