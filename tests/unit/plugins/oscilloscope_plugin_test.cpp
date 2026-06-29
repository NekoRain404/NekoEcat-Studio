// OscilloscopePluginTest — Tests for OscilloscopePlugin
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Service accessor
//   - Channel add/remove with max limit
//   - Timebase and trigger mode configuration
//   - Start/stop acquisition
//   - Acquisition fails closed without synthetic samples
//   - Widget creation and color constants

#include <QTest>
#include <QApplication>
#include <QFile>
#include <QSignalSpy>
#include "plugins/oscilloscope/OscilloscopePlugin.h"
#include "plugins/oscilloscope/OscilloscopeService.h"
#include "plugins/oscilloscope/OscilloscopeWidget.h"

class OscilloscopePluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names
  void testPluginIdentity() {
    OscilloscopeService svc;
    OscilloscopePlugin plugin(&svc);
    QCOMPARE(plugin.id(), QString("oscilloscope"));
    QCOMPARE(plugin.displayName(), QString("Oscilloscope"));
    QCOMPARE(plugin.displayNameZh(), QStringLiteral("示波器"));
  }

  // Plugin has expected default order
  // Verify default order is 100
  void testPluginDefaultOrder() {
    OscilloscopeService svc;
    OscilloscopePlugin plugin(&svc);
    QCOMPARE(plugin.defaultOrder(), 100);
  }

  // Plugin is visible by default
  // Verify plugin is visible
  void testPluginVisible() {
    OscilloscopeService svc;
    OscilloscopePlugin plugin(&svc);
    QVERIFY(plugin.visible());
  }

  // Widget is created and not null
  // Check widget is created
  void testPluginWidgetNotNull() {
    OscilloscopeService svc;
    OscilloscopePlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Service accessor returns injected instance
  // Verify service accessor returns correct pointer
  void testPluginServiceAccessor() {
    OscilloscopeService svc;
    OscilloscopePlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  // Add channel returns valid id and updates channel list
  // Test adding a channel with slave, index, subIndex
  void testServiceAddChannel() {
    OscilloscopeService svc;
    const int id = svc.addChannel(1, "0x6064", "0");
    QVERIFY(id > 0);
    QCOMPARE(svc.channels().size(), 1);
    QCOMPARE(svc.channels().first().slave, 1);
    QCOMPARE(svc.channels().first().index, QString("0x6064"));
    QCOMPARE(svc.channels().first().subIndex, QString("0"));
  }

  // Remove channel decrements channel count
  // Test removing a channel
  void testServiceRemoveChannel() {
    OscilloscopeService svc;
    const int id = svc.addChannel(1, "0x6064", "0");
    QCOMPARE(svc.channels().size(), 1);
    svc.removeChannel(id);
    QCOMPARE(svc.channels().size(), 0);
  }

  // Max 8 channels enforced, overflow returns -1
  // Test max 8 channel limit
  void testServiceMaxChannels() {
    OscilloscopeService svc;
    for (int i = 0; i < 8; ++i) {
      QVERIFY(svc.addChannel(i, "0x6000", "0") > 0);
    }
    QCOMPARE(svc.addChannel(8, "0x6000", "0"), -1);
    QCOMPARE(svc.channels().size(), 8);
  }

  // Timebase can be get and set
  // Test timebase get/set
  void testServiceTimebase() {
    OscilloscopeService svc;
    QCOMPARE(svc.timebase(), 100);
    svc.setTimebase(500);
    QCOMPARE(svc.timebase(), 500);
  }

  // Trigger mode can be get and set
  // Test trigger mode get/set
  void testServiceTriggerMode() {
    OscilloscopeService svc;
    QCOMPARE(svc.triggerMode(), OscTriggerMode::Auto);
    svc.setTriggerMode(OscTriggerMode::Normal);
    QCOMPARE(svc.triggerMode(), OscTriggerMode::Normal);
  }

  // Acquisition start/stop toggles state correctly
  // Test start/stop acquisition
  void testServiceAcquisition() {
    OscilloscopeService svc;
    QVERIFY(!svc.isAcquiring());
    svc.addChannel(0, "0x6000", "0");
    svc.startAcquisition();
    QVERIFY(svc.isAcquiring());
    svc.stopAcquisition();
    QVERIFY(!svc.isAcquiring());
  }

  // Acquisition state may run, but no waveform samples are synthesized without a backend
  void testServiceAcquisitionDoesNotSynthesizeSamples() {
    OscilloscopeService svc;
    const int id = svc.addChannel(0, "0x6000", "0");
    QVERIFY(id > 0);
    QSignalSpy waveSpy(&svc, &OscilloscopeService::waveformUpdated);
    QVERIFY(waveSpy.isValid());

    svc.startAcquisition();
    QTest::qWait(150);
    svc.stopAcquisition();

    QCOMPARE(waveSpy.count(), 0);
    QCOMPARE(svc.channels().size(), 1);
    QVERIFY(svc.channels().first().data.isEmpty());
  }

  void testSourceDoesNotContainSyntheticWaveformGenerator() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp"));
    QVERIFY(source.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("qSin")),
             "Oscilloscope service must not synthesize sine waveforms");
    QVERIFY2(!text.contains(QStringLiteral("QRandomGenerator")),
             "Oscilloscope service must not synthesize noisy samples");
    QVERIFY2(!text.contains(QStringLiteral("tick()")),
             "Oscilloscope service must not keep a synthetic acquisition tick");
  }

  // Widget has minimum width requirement
  // Verify widget minimum width
  void testWidgetCreation() {
    OscilloscopeWidget w;
    QVERIFY(w.minimumSize().width() >= 200);
  }

  // Widget has correct number of channel colors
  // Verify color constants are valid
  void testWidgetColors() {
    QVERIFY(OscilloscopeWidget::kColorCount == 8);
    QVERIFY(OscilloscopeWidget::kColors[0].isValid());
  }
};

QTEST_MAIN(OscilloscopePluginTest)
#include "oscilloscope_plugin_test.moc"
