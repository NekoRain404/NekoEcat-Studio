// EtherCATDocumentationServiceTest — Tests for EtherCATDocumentationService
//
// Test coverage:
//   - Documentation generation (API, user, developer, system)
//   - Documentation format and version
//   - Search functionality (results, no results, relevance)
//   - Signal emission and section structure

#include "services/EtherCATDocumentationService.h"
#include <QSignalSpy>
#include <QTest>

class EtherCATDocumentationServiceTest : public QObject {
    Q_OBJECT
private slots:
    // Verify API documentation has title, content, sections, and author
    void testGenerateApiDocumentation() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto doc = svc.generateApiDocumentation();
        QCOMPARE(doc.title, QStringLiteral("API Documentation"));
        QVERIFY(!doc.content.isEmpty());
        QVERIFY(doc.sections.size() >= 3);
        QCOMPARE(doc.author, QStringLiteral("NekoEcat Team"));
        QVERIFY(doc.timestamp.isValid());
    }

    // Verify user documentation has title and sections
    void testGenerateUserDocumentation() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto doc = svc.generateUserDocumentation();
        QCOMPARE(doc.title, QStringLiteral("User Documentation"));
        QVERIFY(doc.sections.size() >= 3);
    }

    // Verify developer documentation has title and sections
    void testGenerateDeveloperDocumentation() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto doc = svc.generateDeveloperDocumentation();
        QCOMPARE(doc.title, QStringLiteral("Developer Documentation"));
        QVERIFY(doc.sections.size() >= 3);
    }

    // Verify system documentation has title and sections
    void testGenerateSystemDocumentation() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto doc = svc.generateSystemDocumentation();
        QCOMPARE(doc.title, QStringLiteral("System Documentation"));
        QVERIFY(doc.sections.size() >= 3);
    }

    // Verify documentation format is markdown and version is 1.0
    void testDocumentationFormat() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto doc = svc.generateApiDocumentation();
        QCOMPARE(doc.format, QStringLiteral("markdown"));
        QCOMPARE(doc.version, QStringLiteral("1.0"));
    }

    // Verify search finds matching documentation
    void testSearchDocumentation() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto results = svc.searchDocumentation(QStringLiteral("API"));
        QVERIFY(!results.isEmpty());
        bool found = false;
        for (const auto& r : results) {
            if (r.title.contains(QStringLiteral("API"))) {
                found = true;
                break;
            }
        }
        QVERIFY(found);
    }

    // Verify search returns empty for non-existent term
    void testSearchDocumentationNoResults() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto results = svc.searchDocumentation(QStringLiteral("xyznonexistent"));
        QVERIFY(results.isEmpty());
    }

    // Verify search results have positive relevance scores
    void testSearchDocumentationRelevance() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto results = svc.searchDocumentation(QStringLiteral("plugin"));
        QVERIFY(!results.isEmpty());
        for (const auto& r : results)
            QVERIFY(r.relevance > 0.0);
    }

    // Verify documentationGenerated signal is emitted
    void testDocumentationGeneratedSignal() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        QSignalSpy spy(&svc, &EtherCATDocumentationService::documentationGenerated);
        svc.generateApiDocumentation();
        QCOMPARE(spy.count(), 1);
    }

    // Verify all sections have a valid level
    void testSectionsHaveLevel() {
        EtherCATDocumentationService svc(nullptr, nullptr);
        auto doc = svc.generateApiDocumentation();
        for (const auto& section : doc.sections)
            QVERIFY(section.level >= 1);
    }
};

QTEST_MAIN(EtherCATDocumentationServiceTest)
#include "ethercat_documentation_service_test.moc"
