#include <QFile>
#include <QProcessEnvironment>
#include <QString>
#include <QTest>

class WarningsLogTest : public QObject {
  Q_OBJECT

private slots:
  void defaultStudioBuildHasNoKnownWarnings();
};

void WarningsLogTest::defaultStudioBuildHasNoKnownWarnings() {
  const QString logPath =
      QProcessEnvironment::systemEnvironment().value(QStringLiteral("ECAT_BUILD_LOG"));
  if (logPath.isEmpty()) {
    QSKIP("ECAT_BUILD_LOG is not set");
  }

  QFile log(logPath);
  QVERIFY2(log.open(QIODevice::ReadOnly | QIODevice::Text),
           qPrintable(QStringLiteral("Unable to open build log: %1").arg(logPath)));

  const QString text = QString::fromUtf8(log.readAll());
  QVERIFY2(!text.contains(QStringLiteral("-Wsfinae-incomplete")),
           "MOC-generated metatype warnings must be fixed, not hidden in build logs.");
  QVERIFY2(!text.contains(QStringLiteral("-Wdeprecated-declarations")),
           "Qt deprecated API warnings must be fixed before product builds.");
  QVERIFY2(!text.contains(QStringLiteral("QDropEvent::pos() const")),
           "PDO mapping drag/drop should use QDropEvent::position().toPoint().");
}

QTEST_MAIN(WarningsLogTest)
#include "warnings_log_test.moc"
