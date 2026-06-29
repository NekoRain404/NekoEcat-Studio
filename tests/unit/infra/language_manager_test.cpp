// LanguageManagerTest — Tests for LanguageManager
//
// Test coverage:
//   - Default language and display name lookup
//   - Locale code resolution
//   - Language switching by enum and name
//   - Language list and display name retrieval

#include "LanguageManager.h"

#include <QtTest/QtTest>

class LanguageManagerTest : public QObject {
    Q_OBJECT

private slots:
    void defaultLanguageIsEnglish();
    void fromDisplayNameFindsChinese();
    void fromDisplayNameReturnsEnglishForUnknown();
    void fromLocaleCodeFindsChinese();
    void setCurrentLanguageByEnum();
    void setCurrentLanguageByName();
    void languagesReturnsEightEntries();
    void displayNameReturnsCorrectString();
};

// Verify default language is English
void LanguageManagerTest::defaultLanguageIsEnglish()
{
    // LanguageManager is a singleton; reset to English first.
    auto &mgr = LanguageManager::instance();
    mgr.setCurrentLanguage(Language::English);
    QCOMPARE(mgr.currentLanguage(), Language::English);
    QVERIFY(mgr.isCurrentLanguage(Language::English));
}

// Test finding Chinese by display name
void LanguageManagerTest::fromDisplayNameFindsChinese()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.fromDisplayName("简体中文"), Language::ChineseSimplified);
}

// Test unknown display name returns English
void LanguageManagerTest::fromDisplayNameReturnsEnglishForUnknown()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.fromDisplayName("Klingon"), Language::English);
}

// Test finding Chinese by locale code
void LanguageManagerTest::fromLocaleCodeFindsChinese()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.fromLocaleCode("zh-CN"), Language::ChineseSimplified);
}

// Test switching language by enum value
void LanguageManagerTest::setCurrentLanguageByEnum()
{
    auto &mgr = LanguageManager::instance();
    mgr.setCurrentLanguage(Language::ChineseSimplified);
    QVERIFY(mgr.isCurrentLanguage(Language::ChineseSimplified));
    mgr.setCurrentLanguage(Language::English); // reset
}

// Test switching language by display name string
void LanguageManagerTest::setCurrentLanguageByName()
{
    auto &mgr = LanguageManager::instance();
    mgr.setCurrentLanguage("简体中文");
    QCOMPARE(mgr.currentLanguage(), Language::ChineseSimplified);
    mgr.setCurrentLanguage("English"); // reset
}

// Verify language list contains eight entries
void LanguageManagerTest::languagesReturnsEightEntries()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.languages().size(), 8);
}

// Test display name returns correct string for each language
void LanguageManagerTest::displayNameReturnsCorrectString()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.displayName(Language::English), QString("English"));
    QCOMPARE(mgr.displayName(Language::ChineseSimplified), QString("简体中文"));
}

QTEST_MAIN(LanguageManagerTest)
#include "language_manager_test.moc"
