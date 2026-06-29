// ServiceCascadeTest — Integration tests for service error cascade
//
// Test coverage:
//   - Error propagation from EcatClient -> SdoService -> consumer
//   - Error translation (raw daemon errors -> user-friendly messages)
//   - Error recovery after simulated connection loss
//   - Concurrent SDO operations (multiple pending ops)
//   - Service lifecycle (create, use, destroy)

#include <QTest>
#include <QSignalSpy>
#include <QCoreApplication>
#include <QTimer>
#include <QThread>

#include "infra/EcatClient.h"
#include "services/SdoService.h"

class ServiceCascadeTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  SdoService *sdo_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    sdo_ = new SdoService(client_, this);
  }

  void cleanup() {
    delete sdo_;
    sdo_ = nullptr;
    delete client_;
    client_ = nullptr;
  }

  // ── Error Propagation ──────────────────────────────────────────────

  void testErrorCascadeFromClientToService() {
    QSignalSpy spy(sdo_, &SdoService::error);
    QVERIFY(spy.isValid());

    emit client_->errorMessage("SDO timeout: slave did not respond");
    QCOMPARE(spy.count(), 1);

    const QString translated = spy.at(0).at(0).toString();
    QVERIFY(translated.contains("timed out") || translated.contains("timeout"));
    QVERIFY(!translated.isEmpty());
  }

  void testErrorTranslationTimeout() {
    QSignalSpy spy(sdo_, &SdoService::error);

    emit client_->errorMessage("0x05040000");
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toString().contains("timeout"));
  }

  void testErrorTranslationToggleBit() {
    QSignalSpy spy(sdo_, &SdoService::error);

    emit client_->errorMessage("toggle bit not alternated");
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toString().contains("toggle bit"));
  }

  void testErrorTranslationNotConnected() {
    QSignalSpy spy(sdo_, &SdoService::error);

    emit client_->errorMessage("not connected");
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toString().contains("daemon is not connected"));
  }

  void testErrorTranslationUnsupportedAccess() {
    QSignalSpy spy(sdo_, &SdoService::error);

    emit client_->errorMessage("0x06010000");
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toString().contains("unsupported access"));
  }

  void testErrorTranslationReadonlyObject() {
    QSignalSpy spy(sdo_, &SdoService::error);

    emit client_->errorMessage("read only");
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toString().contains("read-only"));
  }

  void testErrorTranslationUnknownError() {
    QSignalSpy spy(sdo_, &SdoService::error);

    emit client_->errorMessage("some unknown error xyz");
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toString().startsWith("SDO error:"));
    QVERIFY(spy.at(0).at(0).toString().contains("some unknown error xyz"));
  }

  // ── SDO Value Forwarding ───────────────────────────────────────────

  void testSdoValueForwarding() {
    QSignalSpy spy(sdo_, &SdoService::sdoValueReceived);
    QVERIFY(spy.isValid());

    emit client_->sdoValue(1, "0x6000", "0x01", "0xFF");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
    QCOMPARE(spy.at(0).at(1).toString(), QString("0x6000"));
    QCOMPARE(spy.at(0).at(2).toString(), QString("0x01"));
    QCOMPARE(spy.at(0).at(3).toString(), QString("0xFF"));
  }

  // ── Error Recovery After Connection Loss ───────────────────────────

  void testErrorRecoveryAfterConnectionLoss() {
    QSignalSpy errorSpy(sdo_, &SdoService::error);
    QSignalSpy valueSpy(sdo_, &SdoService::sdoValueReceived);

    emit client_->errorMessage("not connected");
    QCOMPARE(errorSpy.count(), 1);
    QVERIFY(errorSpy.at(0).at(0).toString().contains("daemon is not connected"));

    emit client_->sdoValue(0, "0x1000", "0x00", "0x12345678");
    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(valueSpy.at(0).at(3).toString(), QString("0x12345678"));
  }

  void testMultipleErrorsThenRecovery() {
    QSignalSpy errorSpy(sdo_, &SdoService::error);
    QSignalSpy valueSpy(sdo_, &SdoService::sdoValueReceived);

    emit client_->errorMessage("SDO timeout");
    emit client_->errorMessage("0x05030000");
    emit client_->errorMessage("not connected");
    QCOMPARE(errorSpy.count(), 3);

    emit client_->sdoValue(0, "0x6000", "0x01", "42");
    QCOMPARE(valueSpy.count(), 1);
  }

  // ── Concurrent Service Operations ──────────────────────────────────

  void testConcurrentSdoUploads() {
    QSignalSpy valueSpy(sdo_, &SdoService::sdoValueReceived);

    sdo_->upload(0, "0x6000", "0x01");
    sdo_->upload(1, "0x6000", "0x02");
    sdo_->upload(2, "0x7000", "0x00");

    emit client_->sdoValue(0, "0x6000", "0x01", "100");
    emit client_->sdoValue(1, "0x6000", "0x02", "200");
    emit client_->sdoValue(2, "0x7000", "0x00", "300");

    QCOMPARE(valueSpy.count(), 3);
    QCOMPARE(valueSpy.at(0).at(3).toString(), QString("100"));
    QCOMPARE(valueSpy.at(1).at(3).toString(), QString("200"));
    QCOMPARE(valueSpy.at(2).at(3).toString(), QString("300"));
  }

  void testConcurrentUploadAndDownload() {
    QSignalSpy valueSpy(sdo_, &SdoService::sdoValueReceived);
    QSignalSpy errorSpy(sdo_, &SdoService::error);

    emit client_->sdoValue(0, "0x6000", "0x01", "42");
    emit client_->errorMessage("SDO access error: this object is write-only");

    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);
    QVERIFY(errorSpy.at(0).at(0).toString().contains("write-only"));
  }

  // ── Service Lifecycle ──────────────────────────────────────────────

  void testServiceCreateUseDestroy() {
    QSignalSpy valueSpy(sdo_, &SdoService::sdoValueReceived);
    QSignalSpy errorSpy(sdo_, &SdoService::error);

    emit client_->sdoValue(0, "0x1000", "0x00", "12345");
    QCOMPARE(valueSpy.count(), 1);

    emit client_->errorMessage("0x06020000");
    QCOMPARE(errorSpy.count(), 1);
    QVERIFY(errorSpy.at(0).at(0).toString().contains("does not exist"));

    emit client_->sdoValue(0, "0x1018", "0x01", "vendor_id");
    QCOMPARE(valueSpy.count(), 2);
  }

  void testServiceRecreation() {
    QSignalSpy valueSpy(sdo_, &SdoService::sdoValueReceived);

    sdo_->upload(0, "0x6000", "0x01");
    emit client_->sdoValue(0, "0x6000", "0x01", "first");
    QCOMPARE(valueSpy.count(), 1);

    delete sdo_;
    sdo_ = new SdoService(client_, this);
    QSignalSpy valueSpy2(sdo_, &SdoService::sdoValueReceived);

    sdo_->upload(0, "0x6000", "0x01");
    emit client_->sdoValue(0, "0x6000", "0x01", "second");
    QCOMPARE(valueSpy2.count(), 1);
    QCOMPARE(valueSpy2.at(0).at(3).toString(), QString("second"));
  }

  // ── Timeout Warning ────────────────────────────────────────────────

  void testTimeoutWarningEmitted() {
    QSignalSpy timeoutSpy(sdo_, &SdoService::sdoTimeoutWarning);
    QVERIFY(timeoutSpy.isValid());

    sdo_->upload(0, "0x6000", "0x01");

    QTest::qWait(6500);

    QVERIFY(timeoutSpy.count() >= 1);
    QCOMPARE(timeoutSpy.at(0).at(0).toInt(), 0);
    QCOMPARE(timeoutSpy.at(0).at(1).toString(), QString("0x6000"));
    QCOMPARE(timeoutSpy.at(0).at(2).toString(), QString("0x01"));
    QVERIFY(timeoutSpy.at(0).at(3).toInt() >= 5000);
  }

  // ── Signal Chaining Through EventBus Path ──────────────────────────

  void testMultipleErrorTranslationsChain() {
    QStringList errors;
    connect(sdo_, &SdoService::error, this, [&errors](const QString &msg) {
      errors.append(msg);
    });

    emit client_->errorMessage("0x05040000");
    emit client_->errorMessage("0x06010001");
    emit client_->errorMessage("0x06090011");

    QCOMPARE(errors.size(), 3);
    QVERIFY(errors[0].contains("timeout"));
    QVERIFY(errors[1].contains("read-only"));
    QVERIFY(errors[2].contains("subindex does not exist"));
  }
};

QTEST_MAIN(ServiceCascadeTest)
#include "service_cascade_test.moc"
