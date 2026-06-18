// Unit tests for LanguageManager — centralized language registry.
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

void LanguageManagerTest::defaultLanguageIsEnglish()
{
    // LanguageManager is a singleton; reset to English first.
    auto &mgr = LanguageManager::instance();
    mgr.setCurrentLanguage(Language::English);
    QCOMPARE(mgr.currentLanguage(), Language::English);
    QVERIFY(mgr.isCurrentLanguage(Language::English));
}

void LanguageManagerTest::fromDisplayNameFindsChinese()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.fromDisplayName("简体中文"), Language::ChineseSimplified);
}

void LanguageManagerTest::fromDisplayNameReturnsEnglishForUnknown()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.fromDisplayName("Klingon"), Language::English);
}

void LanguageManagerTest::fromLocaleCodeFindsChinese()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.fromLocaleCode("zh-CN"), Language::ChineseSimplified);
}

void LanguageManagerTest::setCurrentLanguageByEnum()
{
    auto &mgr = LanguageManager::instance();
    mgr.setCurrentLanguage(Language::ChineseSimplified);
    QVERIFY(mgr.isCurrentLanguage(Language::ChineseSimplified));
    mgr.setCurrentLanguage(Language::English); // reset
}

void LanguageManagerTest::setCurrentLanguageByName()
{
    auto &mgr = LanguageManager::instance();
    mgr.setCurrentLanguage("简体中文");
    QCOMPARE(mgr.currentLanguage(), Language::ChineseSimplified);
    mgr.setCurrentLanguage("English"); // reset
}

void LanguageManagerTest::languagesReturnsEightEntries()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.languages().size(), 8);
}

void LanguageManagerTest::displayNameReturnsCorrectString()
{
    const auto &mgr = LanguageManager::instance();
    QCOMPARE(mgr.displayName(Language::English), QString("English"));
    QCOMPARE(mgr.displayName(Language::ChineseSimplified), QString("简体中文"));
}

QTEST_MAIN(LanguageManagerTest)
#include "language_manager_test.moc"
