// LanguageManager implementation — centralized language registry for 8 languages.
#include "LanguageManager.h"

LanguageManager &LanguageManager::instance()
{
    static LanguageManager mgr;
    return mgr;
}

LanguageManager::LanguageManager()
{
    languages_ = {
        {Language::English,           QStringLiteral("English"),    QStringLiteral("en"),    false},
        {Language::ChineseSimplified, QStringLiteral("简体中文"),   QStringLiteral("zh-CN"), false},
        {Language::Japanese,          QStringLiteral("日本語"),     QStringLiteral("ja"),    false},
        {Language::German,            QStringLiteral("Deutsch"),    QStringLiteral("de"),    false},
        {Language::Korean,            QStringLiteral("한국어"),     QStringLiteral("ko"),    false},
        {Language::ChineseTraditional,QStringLiteral("繁體中文"),   QStringLiteral("zh-TW"), false},
        {Language::French,            QStringLiteral("Français"),   QStringLiteral("fr"),    false},
        {Language::Spanish,           QStringLiteral("Español"),    QStringLiteral("es"),    false},
    };
}

const QVector<LanguageInfo> &LanguageManager::languages() const
{
    return languages_;
}

Language LanguageManager::fromDisplayName(const QString &name) const
{
    for (const auto &info : languages_) {
        if (info.displayName == name) {
            return info.id;
        }
    }
    return Language::English;
}

Language LanguageManager::fromLocaleCode(const QString &code) const
{
    for (const auto &info : languages_) {
        if (info.localeCode == code) {
            return info.id;
        }
    }
    return Language::English;
}

QString LanguageManager::displayName(Language lang) const
{
    for (const auto &info : languages_) {
        if (info.id == lang) {
            return info.displayName;
        }
    }
    return QStringLiteral("English");
}

bool LanguageManager::isCurrentLanguage(Language lang) const
{
    return current_ == lang;
}

Language LanguageManager::currentLanguage() const
{
    return current_;
}

void LanguageManager::setCurrentLanguage(Language lang)
{
    current_ = lang;
}

void LanguageManager::setCurrentLanguage(const QString &displayName)
{
    current_ = fromDisplayName(displayName);
}
