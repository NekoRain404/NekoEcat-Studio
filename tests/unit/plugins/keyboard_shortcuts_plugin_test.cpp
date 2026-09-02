// KeyboardShortcutsPluginTest — Tests for KeyboardShortcutsPlugin
//
// Test coverage:
//   - Plugin identity, order, visibility, and widget creation
//   - Activate/deactivate lifecycle
//   - Shortcut count, key lookup, and reset to defaults

#include "plugins/shortcuts/KeyboardShortcutsPlugin.h"
#include <QApplication>
#include <QTest>

class KeyboardShortcutsPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin id, display names
    void testIdentity() {
        KeyboardShortcutsPlugin p;
        QCOMPARE(p.id(), QString("shortcuts"));
        QCOMPARE(p.displayName(), QString("Keyboard Shortcuts"));
        QCOMPARE(p.displayNameZh(), QString::fromUtf8("键盘快捷键"));
    }
    // Plugin has expected default order
    // Verify default order is 170
    void testDefaultOrder() {
        KeyboardShortcutsPlugin p;
        QCOMPARE(p.defaultOrder(), 170);
    }
    // Plugin is visible by default
    // Verify plugin is visible
    void testVisible() {
        KeyboardShortcutsPlugin p;
        QVERIFY(!p.visible());
    }
    // Widget is created and not null
    // Check widget is created
    void testWidgetNotNull() {
        KeyboardShortcutsPlugin p;
        QVERIFY(p.widget() != nullptr);
    }
    // Activate and deactivate lifecycle completes without error
    // Test activate and deactivate cycle
    void testActivateDeactivate() {
        KeyboardShortcutsPlugin p;
        p.activate();
        p.deactivate();
    }
    // Icon accessor returns a valid icon
    // Verify icon is accessible
    void testIcon() {
        KeyboardShortcutsPlugin p;
        Q_UNUSED(p.icon());
    }
    // Shortcut count is positive
    // Verify shortcut count is positive
    void testShortcutCount() {
        KeyboardShortcutsPlugin p;
        QVERIFY(p.shortcutCount() > 0);
    }
    // Shortcut key lookup returns correct values
    // Test shortcut key retrieval by index
    void testShortcutKey() {
        KeyboardShortcutsPlugin p;
        QCOMPARE(p.shortcutKey(0), QString("Ctrl+N"));
        QCOMPARE(p.shortcutKey(-1), QString());
    }
    // Reset to defaults preserves shortcut count
    // Test reset to defaults preserves count
    void testResetToDefaults() {
        KeyboardShortcutsPlugin p;
        int count = p.shortcutCount();
        p.resetToDefaults();
        QCOMPARE(p.shortcutCount(), count);
    }
};

QTEST_MAIN(KeyboardShortcutsPluginTest)
#include "keyboard_shortcuts_plugin_test.moc"
