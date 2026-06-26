// WorkflowCertificationManagerServiceTest — Tests for WorkflowCertificationManagerService
//
// Test coverage:
//   - Requirement CRUD (add, remove, get)
//   - Status update
//   - Standard filtering
//   - Renewal
//   - Pending count
//   - Signal emissions

#include <QTest>
#include <QSignalSpy>
#include <QFile>
#include "services/WorkflowCertificationManagerService.h"

class WorkflowCertificationManagerServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddRequirement() {
      WorkflowCertificationManagerService svc;
      QSignalSpy spy(&svc, &WorkflowCertificationManagerService::requirementAdded);
      auto id = svc.addRequirement(QStringLiteral("ISO 9001 Certification"),
                                   QStringLiteral("ISO 9001"));
      QVERIFY(!id.isEmpty());
      QCOMPARE(spy.count(), 1);
      QCOMPARE(svc.requirementCount(), 1);
      QVERIFY(!svc.requirement(id).expiry.isValid());
  }

  void testRemoveRequirement() {
      WorkflowCertificationManagerService svc;
      auto id = svc.addRequirement(QStringLiteral("Test"), QStringLiteral("Std"));
      QSignalSpy spy(&svc, &WorkflowCertificationManagerService::requirementRemoved);
      QVERIFY(svc.removeRequirement(id));
      QCOMPARE(spy.count(), 1);
      QCOMPARE(svc.requirementCount(), 0);
  }

  void testRemoveNonexistent() {
      WorkflowCertificationManagerService svc;
      QVERIFY(!svc.removeRequirement(QStringLiteral("bad")));
  }

  void testUpdateStatus() {
      WorkflowCertificationManagerService svc;
      auto id = svc.addRequirement(QStringLiteral("Test"), QStringLiteral("Std"));
      QSignalSpy spy(&svc, &WorkflowCertificationManagerService::statusUpdated);
      QVERIFY(!svc.updateStatus(id, QStringLiteral("approved")));
      QCOMPARE(spy.count(), 0);
      QCOMPARE(svc.requirement(id).status, QStringLiteral("pending"));
      QVERIFY(svc.updateStatus(id, QStringLiteral("blocked")));
      QCOMPARE(svc.requirement(id).status, QStringLiteral("blocked"));
  }

  void testUpdateStatusNonexistent() {
      WorkflowCertificationManagerService svc;
      QVERIFY(!svc.updateStatus(QStringLiteral("bad"), QStringLiteral("approved")));
  }

  void testGetRequirement() {
      WorkflowCertificationManagerService svc;
      auto id = svc.addRequirement(QStringLiteral("CE Mark"),
                                   QStringLiteral("CE Directive"));
      auto r = svc.requirement(id);
      QCOMPARE(r.name, QStringLiteral("CE Mark"));
      QCOMPARE(r.standard, QStringLiteral("CE Directive"));
      QCOMPARE(r.status, QStringLiteral("pending"));
  }

  void testGetNonexistent() {
      WorkflowCertificationManagerService svc;
      auto r = svc.requirement(QStringLiteral("bad"));
      QVERIFY(r.id.isEmpty());
  }

  void testAllRequirements() {
      WorkflowCertificationManagerService svc;
      svc.addRequirement(QStringLiteral("R1"), QStringLiteral("S1"));
      svc.addRequirement(QStringLiteral("R2"), QStringLiteral("S2"));
      svc.addRequirement(QStringLiteral("R3"), QStringLiteral("S3"));
      QCOMPARE(svc.allRequirements().size(), 3);
  }

  void testRequirementsByStandard() {
      WorkflowCertificationManagerService svc;
      svc.addRequirement(QStringLiteral("R1"), QStringLiteral("ISO"));
      svc.addRequirement(QStringLiteral("R2"), QStringLiteral("CE"));
      svc.addRequirement(QStringLiteral("R3"), QStringLiteral("ISO"));
      auto isoReqs = svc.requirementsByStandard(QStringLiteral("ISO"));
      QCOMPARE(isoReqs.size(), 2);
      auto ceReqs = svc.requirementsByStandard(QStringLiteral("CE"));
      QCOMPARE(ceReqs.size(), 1);
  }

  void testRenewRequirement() {
      WorkflowCertificationManagerService svc;
      auto id = svc.addRequirement(QStringLiteral("Test"), QStringLiteral("Std"));
      QDateTime newExpiry = QDateTime::currentDateTime().addYears(2);
      QSignalSpy spy(&svc, &WorkflowCertificationManagerService::requirementRenewed);
      QVERIFY(!svc.renewRequirement(id, newExpiry));
      QCOMPARE(spy.count(), 0);
      QCOMPARE(svc.requirement(id).status, QStringLiteral("pending"));
      QVERIFY(!svc.requirement(id).expiry.isValid());
  }

  void testRenewNonexistent() {
      WorkflowCertificationManagerService svc;
      QVERIFY(!svc.renewRequirement(QStringLiteral("bad"), QDateTime::currentDateTime()));
  }

  void testPendingCount() {
      WorkflowCertificationManagerService svc;
      svc.addRequirement(QStringLiteral("R1"), QStringLiteral("S1"));
      svc.addRequirement(QStringLiteral("R2"), QStringLiteral("S2"));
      auto id3 = svc.addRequirement(QStringLiteral("R3"), QStringLiteral("S3"));
      svc.updateStatus(id3, QStringLiteral("blocked"));
      QCOMPARE(svc.pendingCount(), 2);
  }

  void testCustomExpiry() {
      WorkflowCertificationManagerService svc;
      QDateTime customExpiry = QDateTime::currentDateTime().addMonths(6);
      auto id = svc.addRequirement(QStringLiteral("Test"), QStringLiteral("Std"),
                                   customExpiry);
      auto r = svc.requirement(id);
      QVERIFY(r.expiry.isValid());
  }

  void testSourceDoesNotSynthesizeCertificationValidity() {
      QFile file(QStringLiteral(SOURCE_ROOT
                                "/apps/ecat-studio/services/WorkflowCertificationManagerService.cpp"));
      QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
               qPrintable(file.errorString()));
      const QString source = QString::fromUtf8(file.readAll());

      QVERIFY2(!source.contains(QStringLiteral("createdAt.addYears(1)")),
               "Certification manager must not synthesize default certificate expiry");
      QVERIFY2(!source.contains(QStringLiteral("r.status = QStringLiteral(\"renewed\")")),
               "Certification manager must not renew requirements without a backend");
  }
};

QTEST_MAIN(WorkflowCertificationManagerServiceTest)
#include "workflow_certification_manager_service_test.moc"
