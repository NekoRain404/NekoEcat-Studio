// UserPreferencesPluginTest — Tests for UserPreferencesPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Default order and visibility
//   - Widget creation, activate/deactivate lifecycle
//   - Icon access and reset-to-defaults

#include <QTest>
#include <QApplication>
#include "plugins/preferences/UserPreferencesPlugin.h"

class UserPreferencesPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Plugin id, display name, and Chinese name
  void testIdentity() {
    UserPreferencesPlugin p;
    QCOMPARE(p.id(), QString("preferences"));
    QCOMPARE(p.displayName(), QString("User Preferences"));
    QCOMPARE(p.displayNameZh(), QString::fromUtf8("用户偏好"));
  }
  // Default order value is 175
  void testDefaultOrder() {
    UserPreferencesPlugin p;
    QCOMPARE(p.defaultOrder(), 175);
  }
  // Plugin is visible by default
  void testVisible() {
    UserPreferencesPlugin p;
    QVERIFY(p.visible());
  }
  // Widget is created and non-null
  void testWidgetNotNull() {
    UserPreferencesPlugin p;
    QVERIFY(p.widget() != nullptr);
  }
  // Activate and deactivate complete without error
  void testActivateDeactivate() {
    UserPreferencesPlugin p;
    p.activate();
    p.deactivate();
  }
  // Icon accessor returns without error
  void testIcon() {
    UserPreferencesPlugin p;
    Q_UNUSED(p.icon());
  }
  // Reset restores defaults and widget remains valid
  void testResetToDefaults() {
    UserPreferencesPlugin p;
    p.resetToDefaults();
    QVERIFY(p.widget() != nullptr);
  }
};

QTEST_MAIN(UserPreferencesPluginTest)
#include "user_preferences_plugin_test.moc"
