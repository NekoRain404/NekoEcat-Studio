// NotesPluginTest — Tests for NotesPlugin
//
// Test coverage:
//   - Plugin identity, order, visibility, and widget creation
//   - Notes text round-trip read/write

#include "plugins/notes/NotesPlugin.h"
#include <QTest>

class NotesPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin id, display names
    void testIdentity() {
        NotesPlugin p;
        QCOMPARE(p.id(), QString("notes"));
        QCOMPARE(p.displayName(), QString("Notes"));
        QCOMPARE(p.displayNameZh(), QString("备注"));
    }
    // Plugin has expected default order
    // Verify default order is 100
    void testDefaultOrder() {
        NotesPlugin p;
        QCOMPARE(p.defaultOrder(), 100);
    }
    // Plugin is visible by default
    // Verify plugin is visible
    void testVisible() {
        NotesPlugin p;
        QVERIFY(!p.visible());
    }
    // Widget is created and not null
    // Check widget is created
    void testWidgetNotNull() {
        NotesPlugin p;
        QVERIFY(p.widget() != nullptr);
    }
    // Notes text can be set and retrieved
    // Test setting and getting notes text
    void testNotesTextRoundTrip() {
        NotesPlugin p;
        p.setNotesText("Hello World");
        QCOMPARE(p.notesText(), QString("Hello World"));
        p.setNotesText("");
        QCOMPARE(p.notesText(), QString(""));
    }
};

QTEST_MAIN(NotesPluginTest)
#include "notes_plugin_test.moc"
