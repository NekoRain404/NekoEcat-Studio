#include <QTest>
#include "plugins/notes/NotesPlugin.h"

class NotesPluginTest : public QObject {
  Q_OBJECT
private slots:
  void testIdentity() {
    NotesPlugin p;
    QCOMPARE(p.id(), QString("notes"));
    QCOMPARE(p.displayName(), QString("Notes"));
    QCOMPARE(p.displayNameZh(), QString("备注"));
  }
  void testDefaultOrder() {
    NotesPlugin p;
    QCOMPARE(p.defaultOrder(), 100);
  }
  void testVisible() {
    NotesPlugin p;
    QVERIFY(p.visible());
  }
  void testWidgetNotNull() {
    NotesPlugin p;
    QVERIFY(p.widget() != nullptr);
  }
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
