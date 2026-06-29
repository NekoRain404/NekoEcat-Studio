// UITestFixtureTest — Tests for UITestFixture utility
//
// Test coverage:
//   - Widget creation and existence tracking
//   - Widget destruction
//   - Multiple widget management

#include <QTest>
#include "fixtures/UITestFixture.h"

class UITestFixtureTest : public QObject {
    Q_OBJECT
private slots:
    // Create widget returns non-null and tracks existence
    void testCreateWidget() {
        UITestFixture fixture;
        QWidget *w = fixture.createWidget();
        QVERIFY(w != nullptr);
        QVERIFY(fixture.widgetExists(w));
    }

    // Destroy widget removes it from tracking
    void testDestroyWidget() {
        UITestFixture fixture;
        QWidget *w = fixture.createWidget();
        QVERIFY(fixture.widgetExists(w));
        fixture.destroyWidget(w);
        QVERIFY(!fixture.widgetExists(w));
    }

    // Multiple widgets tracked independently; removing one preserves others
    void testMultipleWidgets() {
        UITestFixture fixture;
        QWidget *w1 = fixture.createWidget();
        QWidget *w2 = fixture.createWidget();
        QWidget *w3 = fixture.createWidget();
        QVERIFY(fixture.widgetExists(w1));
        QVERIFY(fixture.widgetExists(w2));
        QVERIFY(fixture.widgetExists(w3));
        fixture.destroyWidget(w2);
        QVERIFY(!fixture.widgetExists(w2));
        QVERIFY(fixture.widgetExists(w1));
        QVERIFY(fixture.widgetExists(w3));
    }
};

QTEST_MAIN(UITestFixtureTest)
#include "ui_fixture_test.moc"
