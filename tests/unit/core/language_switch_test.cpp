// language_switch_test.cpp
//
// End-to-end GUI language mapping tests for LanguageManager + TranslationRegistry.
//
// Coverage:
//   - For all 7 non-English languages: fromDisplayName() -> enum, then
//     translationFileSuffix() / localeCode() produce the expected .qm basename
//     suffix and BCP-47 code (hard-coded expectations).
//   - fromLocaleCode() legacy fallback ("zh-CN", "zh-TW", ...) plus unknown codes.
//   - switchToLanguage emits languageChanged and currentLanguage reflects it.
//   - Round trip: set to 简体中文, read back, switch to English, no crash.
//   - TranslationRegistry::translate() mapped value + English passthrough.

#include "LanguageManager.h"
#include "TranslationRegistry.h"

#include <QtTest/QtTest>

class LanguageSwitchTest : public QObject {
    Q_OBJECT

    struct Expectation {
        const char* display;
        Language lang;
        const char* suffix; // translation-file basename suffix (e.g. "zh")
        const char* bcp47;  // localeCode (e.g. "zh-CN")
    };

    // Expected suffix / BCP-47 code per non-English language, matching the
    // shipped LanguageManager implementation.
    static const Expectation kExpected[7];

private slots:
    void displayNameToEnumAndFiles();
    void legacyLocaleFallback();
    void switchEmitsSignalAndReflects();
    void roundTrip();
    void registryTranslate();
};

const LanguageSwitchTest::Expectation LanguageSwitchTest::kExpected[7] = {
    {"简体中文", Language::ChineseSimplified, "zh", "zh-CN"},
    {"日本語", Language::Japanese, "ja", "ja"},
    {"Deutsch", Language::German, "de", "de"},
    {"한국어", Language::Korean, "ko", "ko"},
    {"繁體中文", Language::ChineseTraditional, "zh_TW", "zh-TW"},
    {"Français", Language::French, "fr", "fr"},
    {"Español", Language::Spanish, "es", "es"},
};

void LanguageSwitchTest::displayNameToEnumAndFiles() {
    auto& mgr = LanguageManager::instance();

    for (const auto& e : kExpected) {
        const Language lang = mgr.fromDisplayName(QString::fromUtf8(e.display));
        QCOMPARE(lang, e.lang);

        QCOMPARE(mgr.translationFileSuffix(lang), QString::fromUtf8(e.suffix));

        // localeCode lives on the LanguageInfo entry in the languages() list.
        bool found = false;
        for (const auto& info : mgr.languages()) {
            if (info.id == e.lang) {
                QCOMPARE(info.localeCode, QString::fromUtf8(e.bcp47));
                found = true;
            }
        }
        QVERIFY2(found, QString("localeCode entry missing for %1").arg(e.display).toUtf8());
    }

    // English basename is "en" (used for the source .ts file).
    QCOMPARE(mgr.translationFileSuffix(Language::English), QString("en"));

    // Unknown display names fall back to English.
    QCOMPARE(mgr.fromDisplayName("Klingon"), Language::English);
}

void LanguageSwitchTest::legacyLocaleFallback() {
    const auto& mgr = LanguageManager::instance();

    QCOMPARE(mgr.fromLocaleCode("zh-CN"), Language::ChineseSimplified);
    QCOMPARE(mgr.fromLocaleCode("zh-TW"), Language::ChineseTraditional);
    QCOMPARE(mgr.fromLocaleCode("ja"), Language::Japanese);
    QCOMPARE(mgr.fromLocaleCode("de"), Language::German);
    QCOMPARE(mgr.fromLocaleCode("ko"), Language::Korean);
    QCOMPARE(mgr.fromLocaleCode("fr"), Language::French);
    QCOMPARE(mgr.fromLocaleCode("es"), Language::Spanish);
    QCOMPARE(mgr.fromLocaleCode("en"), Language::English);

    // Unknown / malformed codes fall back to English.
    QCOMPARE(mgr.fromLocaleCode("xx-YY"), Language::English);
    QCOMPARE(mgr.fromLocaleCode(""), Language::English);
}

void LanguageSwitchTest::switchEmitsSignalAndReflects() {
    auto& mgr = LanguageManager::instance();
    mgr.setCurrentLanguage(Language::English); // reset baseline

    // Use QSignalSpy: a plain lambda would capture stack locals that dangle
    // after this slot returns while remaining connected to the singleton.
    QSignalSpy spy(&mgr, &LanguageManager::languageChanged);

    mgr.setCurrentLanguage(Language::Japanese);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).value<Language>(), Language::Japanese);
    QCOMPARE(mgr.currentLanguage(), Language::Japanese);
    QVERIFY(mgr.isCurrentLanguage(Language::Japanese));

    // Switching to the same language must not re-emit.
    mgr.setCurrentLanguage(Language::Japanese);
    QCOMPARE(spy.count(), 0);

    // Setting the same language by display name also does not re-emit.
    mgr.setCurrentLanguage("日本語");
    QCOMPARE(spy.count(), 0);

    // Switch back by display name (QString overload).
    mgr.setCurrentLanguage("简体中文");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).value<Language>(), Language::ChineseSimplified);
    QCOMPARE(mgr.currentLanguage(), Language::ChineseSimplified);

    mgr.setCurrentLanguage(Language::English); // cleanup
}

void LanguageSwitchTest::roundTrip() {
    auto& mgr = LanguageManager::instance();
    mgr.setCurrentLanguage(Language::English);

    mgr.setCurrentLanguage("简体中文"); // switch to zh by display name
    QCOMPARE(mgr.currentLanguage(), Language::ChineseSimplified);
    QCOMPARE(mgr.displayName(mgr.currentLanguage()), QString::fromUtf8("简体中文"));

    // read back through the by-name path as the settings dialog would
    mgr.setCurrentLanguage(mgr.displayName(mgr.currentLanguage()));
    QCOMPARE(mgr.currentLanguage(), Language::ChineseSimplified);
    QVERIFY(!mgr.isCurrentLanguage(Language::English));

    // back to English: must not crash and must reflect.
    mgr.setCurrentLanguage(Language::English);
    QCOMPARE(mgr.currentLanguage(), Language::English);
    QVERIFY(mgr.isCurrentLanguage(Language::English));
    QVERIFY(!mgr.isCurrentLanguage(Language::ChineseSimplified));
}

void LanguageSwitchTest::registryTranslate() {
    auto& reg = TranslationRegistry::instance();

    // English is passthrough.
    QCOMPARE(reg.translate("About", Language::English), QString("About"));

    // Every non-English language has a non-empty translation for "About".
    for (const auto& e : kExpected) {
        const QString translated = reg.translate("About", e.lang);
        QVERIFY2(!translated.isEmpty() && translated != "About",
                 QString("missing translation for %1").arg(e.display).toUtf8());
    }

    // Known mapping sanity checks (verified against the shipped .cpp maps).
    QCOMPARE(reg.translate("About", Language::Japanese), QString::fromUtf8("について"));
    QCOMPARE(reg.translate("Add", Language::ChineseSimplified), QString::fromUtf8("添加"));
    QCOMPARE(reg.translate("About", Language::German), QString::fromUtf8("Über"));

    // Untranslated keys fall back to the English key.
    QCOMPARE(reg.translate("zznonexistent_zz", Language::Korean), QString("zznonexistent_zz"));
}

QTEST_MAIN(LanguageSwitchTest)
#include "language_switch_test.moc"