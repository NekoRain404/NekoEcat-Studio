// ThemeCustomizerPluginTest — Tests for ThemeCustomizerPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Default order and visibility
//   - Widget creation, activate/deactivate lifecycle
//   - Icon access and reset-to-defaults

#include <QTest>
#include <QApplication>
#include "plugins/themecustomizer/ThemeCustomizerPlugin.h"

class ThemeCustomizerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Plugin id, display name, and Chinese name
  void testIdentity() {
    ThemeCustomizerPlugin p;
    QCOMPARE(p.id(), QString("themecustomizer"));
    QCOMPARE(p.displayName(), QString("Theme Customizer"));
    QCOMPARE(p.displayNameZh(), QString::fromUtf8("主题定制器"));
  }
  // Default order value is 165
  void testDefaultOrder() {
    ThemeCustomizerPlugin p;
    QCOMPARE(p.defaultOrder(), 165);
  }
  // Plugin is visible by default
  void testVisible() {
    ThemeCustomizerPlugin p;
    QVERIFY(p.visible());
  }
  // Widget is created and non-null
  void testWidgetNotNull() {
    ThemeCustomizerPlugin p;
    QVERIFY(p.widget() != nullptr);
  }
  // Activate and deactivate complete without error
  void testActivateDeactivate() {
    ThemeCustomizerPlugin p;
    p.activate();
    p.deactivate();
  }
  // Icon accessor returns without error
  void testIcon() {
    ThemeCustomizerPlugin p;
    Q_UNUSED(p.icon());
  }
  // Reset restores defaults and widget remains valid
  void testResetToDefaults() {
    ThemeCustomizerPlugin p;
    p.resetToDefaults();
    QVERIFY(p.widget() != nullptr);
  }
};

QTEST_MAIN(ThemeCustomizerPluginTest)
#include "theme_customizer_plugin_test.moc"
