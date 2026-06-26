#include <QFile>
#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include "services/WorkflowReportDesignerService.h"

class WorkflowReportDesignerServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testCreateTemplate() {
    WorkflowReportDesignerService svc;
    QSignalSpy spy(&svc, &WorkflowReportDesignerService::templateCreated);
    QString id = svc.createTemplate("Test Template", "A test", {"Summary"}, {"data"});
    QVERIFY(!id.isEmpty());
    QVERIFY(id.startsWith("tmpl_"));
    QCOMPARE(spy.count(), 1);
  }

  void testRemoveTemplate() {
    WorkflowReportDesignerService svc;
    QString id = svc.createTemplate("ToRemove", "Desc", {"S"}, {"D"});
    QSignalSpy spy(&svc, &WorkflowReportDesignerService::templateRemoved);
    QVERIFY(svc.removeTemplate(id));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.templateCount(), 0);
  }

  void testUpdateTemplate() {
    WorkflowReportDesignerService svc;
    QString id = svc.createTemplate("Old Name", "Desc", {"S1"}, {"D"});
    QSignalSpy spy(&svc, &WorkflowReportDesignerService::templateUpdated);
    QVERIFY(svc.updateTemplate(id, "New Name", {"S1", "S2"}));
    QCOMPARE(spy.count(), 1);
    WfReportTemplate t = svc.templateById(id);
    QCOMPARE(t.name, QString("New Name"));
    QCOMPARE(t.sections.size(), 2);
  }

  void testTemplateLookup() {
    WorkflowReportDesignerService svc;
    QString id = svc.createTemplate("Lookup", "Desc", {"S"}, {"D"});
    WfReportTemplate t = svc.templateById(id);
    QCOMPARE(t.id, id);
    QCOMPARE(t.name, QString("Lookup"));
    QCOMPARE(t.sections.size(), 1);
  }

  void testAllTemplates() {
    WorkflowReportDesignerService svc;
    svc.createTemplate("T1", "D", {"S"}, {"F"});
    svc.createTemplate("T2", "D", {"S"}, {"F"});
    QCOMPARE(svc.allTemplates().size(), 2);
  }

  void testGenerateReport() {
    WorkflowReportDesignerService svc;
    QString tmplId = svc.createTemplate("Gen", "Desc", {"Summary", "Details"}, {"val"});
    QJsonObject data;
    data["Summary"] = "Test summary";
    QSignalSpy spy(&svc, &WorkflowReportDesignerService::reportGenerated);
    WfCustomReport r = svc.generateReport(tmplId, data);
    QVERIFY(r.success);
    QVERIFY(!r.id.isEmpty());
    QVERIFY(r.id.startsWith("rpt_"));
    QCOMPARE(r.templateId, tmplId);
    QCOMPARE(r.sections.size(), 2);
    QCOMPARE(spy.count(), 1);
  }

  void testGenerateReportInvalid() {
    WorkflowReportDesignerService svc;
    WfCustomReport r = svc.generateReport("nonexistent", {});
    QVERIFY(!r.success);
  }

  void testCustomReportDefaultsToFailure() {
    WfCustomReport report;
    QVERIFY(!report.success);
  }

  void testAllReports() {
    WorkflowReportDesignerService svc;
    QCOMPARE(svc.allReports().size(), 0);
    QString tmplId = svc.createTemplate("T", "D", {"S"}, {});
    svc.generateReport(tmplId, {});
    svc.generateReport(tmplId, {});
    QCOMPARE(svc.allReports().size(), 2);
  }

  void testReportIdFormat() {
    WorkflowReportDesignerService svc;
    QString tmplId = svc.createTemplate("T", "D", {"S"}, {});
    WfCustomReport r = svc.generateReport(tmplId, {});
    QVERIFY(r.id.contains("rpt_"));
  }

  void testReportSectionsHaveContent() {
    WorkflowReportDesignerService svc;
    QString tmplId = svc.createTemplate("T", "D", {"S1", "S2"}, {});
    WfCustomReport r = svc.generateReport(tmplId, {});
    for (const auto &s : r.sections) {
      QVERIFY(!s.first.isEmpty());
      QVERIFY(!s.second.isEmpty());
    }
  }

  void testTemplateNotFound() {
    WorkflowReportDesignerService svc;
    WfReportTemplate t = svc.templateById("nonexistent");
    QVERIFY(t.id.isEmpty());
  }

  void testSignalEmissions() {
    WorkflowReportDesignerService svc;
    QSignalSpy tcSpy(&svc, &WorkflowReportDesignerService::templateCreated);
    QSignalSpy trSpy(&svc, &WorkflowReportDesignerService::templateRemoved);
    QSignalSpy tuSpy(&svc, &WorkflowReportDesignerService::templateUpdated);
    QSignalSpy rgSpy(&svc, &WorkflowReportDesignerService::reportGenerated);

    QString id = svc.createTemplate("T", "D", {"S"}, {});
    svc.updateTemplate(id, "T2", {"S1", "S2"});
    svc.generateReport(id, {});
    svc.removeTemplate(id);

    QCOMPARE(tcSpy.count(), 1);
    QCOMPARE(tuSpy.count(), 1);
    QCOMPARE(rgSpy.count(), 1);
    QCOMPARE(trSpy.count(), 1);
  }

  void noDefaultSyntheticSuccessInSource() {
    QFile header(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/WorkflowReportDesignerService.h"));
    QVERIFY2(header.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(QStringLiteral("Unable to open %1").arg(header.fileName())));
    const QString text = QString::fromUtf8(header.readAll());
    QVERIFY2(!text.contains(QStringLiteral("bool success = true")),
             "Custom report results must not default to success.");
  }
};

QTEST_MAIN(WorkflowReportDesignerServiceTest)
#include "workflow_report_designer_service_test.moc"
