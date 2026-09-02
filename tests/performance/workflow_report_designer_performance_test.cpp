#include "services/WorkflowReportDesignerService.h"
#include <QElapsedTimer>
#include <QJsonObject>
#include <QTest>

class WorkflowReportDesignerPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testCreateTemplateThroughput() {
        WorkflowReportDesignerService svc;
        QElapsedTimer timer;
        timer.start();
        const int count = 10000;
        for (int i = 0; i < count; i++) {
            svc.createTemplate(QString("Tmpl_%1").arg(i), "Test", {"Summary"}, {"data"});
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "CreateTemplate throughput:" << count << "templates in" << elapsed << "ms";
    }

    void testGenerateReportThroughput() {
        WorkflowReportDesignerService svc;
        QString tmplId = svc.createTemplate("PerfTest", "Test", {"S1", "S2"}, {"d1"});
        QJsonObject data;
        data["d1"] = "value1";
        QElapsedTimer timer;
        timer.start();
        const int count = 10000;
        for (int i = 0; i < count; i++) {
            svc.generateReport(tmplId, data);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "GenerateReport throughput:" << count << "reports in" << elapsed << "ms";
    }

    void testQueryThroughput() {
        WorkflowReportDesignerService svc;
        for (int i = 0; i < 100; i++) {
            svc.createTemplate(QString("T_%1").arg(i), "Test", {"S"}, {"d"});
        }
        QElapsedTimer timer;
        timer.start();
        const int count = 10000;
        for (int i = 0; i < count; i++) {
            svc.allTemplates();
            svc.allReports();
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Query throughput:" << count << "queries in" << elapsed << "ms";
    }

    void testUpdateTemplateThroughput() {
        WorkflowReportDesignerService svc;
        QVector<QString> ids;
        for (int i = 0; i < 1000; i++) {
            ids << svc.createTemplate(QString("T_%1").arg(i), "Test", {"S"}, {"d"});
        }
        QElapsedTimer timer;
        timer.start();
        for (const auto& id : ids) {
            svc.updateTemplate(id, "Updated", {"NewSection"});
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "UpdateTemplate throughput:" << ids.size() << "updates in" << elapsed << "ms";
    }
};

QTEST_MAIN(WorkflowReportDesignerPerformanceTest)
#include "workflow_report_designer_performance_test.moc"
