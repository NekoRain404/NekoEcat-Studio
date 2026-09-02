// WorkflowMaintenanceServiceTest — Tests for Workflow Maintenance Service
//
// Test coverage:
//   - Schedule maintenance task
//   - Execute maintenance task fails closed without maintenance backend
//   - Execute nonexistent task returns false
//   - Empty description returns false
//   - Auto-assigned task IDs
//   - Explicit task IDs
//   - Maintenance history tracking
//   - Maintenance schedule listing
//   - Signal emissions

#include "services/WorkflowMaintenanceService.h"
#include <QSignalSpy>
#include <QTest>

class WorkflowMaintenanceServiceTest : public QObject {
    Q_OBJECT
private slots:
    void testScheduleMaintenance() {
        WorkflowMaintenanceService svc;
        QSignalSpy spy(&svc, &WorkflowMaintenanceService::maintenanceScheduled);

        WfMaintenanceTask task;
        task.type = WfMaintenanceType::Preventive;
        task.description = QStringLiteral("Check cable integrity");
        task.priority = 5;
        task.estimatedDurationMin = 30;

        QVERIFY(svc.scheduleMaintenance(task));
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).value<WfMaintenanceTask>().description, QString("Check cable integrity"));
    }

    void testExecuteMaintenance() {
        WorkflowMaintenanceService svc;
        QSignalSpy spy(&svc, &WorkflowMaintenanceService::maintenanceCompleted);

        WfMaintenanceTask task;
        task.description = QStringLiteral("Replace connector");
        svc.scheduleMaintenance(task);

        auto tasks = svc.maintenanceSchedule();
        QCOMPARE(tasks.size(), 1);

        QVERIFY(!svc.executeMaintenance(tasks[0].taskId));
        QCOMPARE(spy.count(), 0);
        QCOMPARE(svc.maintenanceHistory().size(), 0);
    }

    void testExecuteNonexistentReturnsFalse() {
        WorkflowMaintenanceService svc;
        QVERIFY(!svc.executeMaintenance(999));
    }

    void testEmptyDescriptionReturnsFalse() {
        WorkflowMaintenanceService svc;
        WfMaintenanceTask task;
        QVERIFY(!svc.scheduleMaintenance(task));
    }

    void testAutoAssignedTaskIds() {
        WorkflowMaintenanceService svc;

        WfMaintenanceTask t1;
        t1.description = QStringLiteral("Task 1");
        svc.scheduleMaintenance(t1);

        WfMaintenanceTask t2;
        t2.description = QStringLiteral("Task 2");
        svc.scheduleMaintenance(t2);

        auto tasks = svc.maintenanceSchedule();
        QCOMPARE(tasks.size(), 2);
        QVERIFY(tasks[0].taskId != tasks[1].taskId);
        QCOMPARE(tasks[0].taskId, 1);
        QCOMPARE(tasks[1].taskId, 2);
    }

    void testExplicitTaskId() {
        WorkflowMaintenanceService svc;

        WfMaintenanceTask task;
        task.taskId = 42;
        task.description = QStringLiteral("Explicit ID task");
        svc.scheduleMaintenance(task);

        auto tasks = svc.maintenanceSchedule();
        QCOMPARE(tasks.size(), 1);
        QCOMPARE(tasks[0].taskId, 42);
    }

    void testMaintenanceHistory() {
        WorkflowMaintenanceService svc;

        WfMaintenanceTask task;
        task.description = QStringLiteral("History task");
        svc.scheduleMaintenance(task);

        auto tasks = svc.maintenanceSchedule();
        svc.executeMaintenance(tasks[0].taskId);

        auto history = svc.maintenanceHistory();
        QCOMPARE(history.size(), 0);
    }

    void testMaintenanceSchedule() {
        WorkflowMaintenanceService svc;

        WfMaintenanceTask t1;
        t1.description = QStringLiteral("Preventive check");
        t1.type = WfMaintenanceType::Preventive;
        svc.scheduleMaintenance(t1);

        WfMaintenanceTask t2;
        t2.description = QStringLiteral("Predictive analysis");
        t2.type = WfMaintenanceType::Predictive;
        svc.scheduleMaintenance(t2);

        auto schedule = svc.maintenanceSchedule();
        QCOMPARE(schedule.size(), 2);
        QCOMPARE(schedule[0].type, WfMaintenanceType::Preventive);
        QCOMPARE(schedule[1].type, WfMaintenanceType::Predictive);
    }

    void testMaintenanceTypes() {
        WorkflowMaintenanceService svc;

        WfMaintenanceTask t1;
        t1.description = QStringLiteral("Preventive");
        t1.type = WfMaintenanceType::Preventive;
        svc.scheduleMaintenance(t1);

        WfMaintenanceTask t2;
        t2.description = QStringLiteral("Corrective");
        t2.type = WfMaintenanceType::Corrective;
        svc.scheduleMaintenance(t2);

        WfMaintenanceTask t3;
        t3.description = QStringLiteral("Predictive");
        t3.type = WfMaintenanceType::Predictive;
        svc.scheduleMaintenance(t3);

        WfMaintenanceTask t4;
        t4.description = QStringLiteral("Scheduled");
        t4.type = WfMaintenanceType::Scheduled;
        svc.scheduleMaintenance(t4);

        auto schedule = svc.maintenanceSchedule();
        QCOMPARE(schedule.size(), 4);
    }

    void testRecordTimestamps() {
        WorkflowMaintenanceService svc;

        WfMaintenanceTask task;
        task.description = QStringLiteral("Timestamp task");
        svc.scheduleMaintenance(task);

        auto tasks = svc.maintenanceSchedule();
        svc.executeMaintenance(tasks[0].taskId);

        auto history = svc.maintenanceHistory();
        QCOMPARE(history.size(), 0);
    }

    void testSourceDoesNotMintSyntheticMaintenanceSuccess() {
        QFile file(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/WorkflowMaintenanceService.cpp"));
        QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text), qPrintable(file.errorString()));
        const QString source = QString::fromUtf8(file.readAll());

        QVERIFY2(!source.contains(QStringLiteral("record.success = true")),
                 "Maintenance execution must not synthesize success without a maintenance backend");
        QVERIFY2(!source.contains(QStringLiteral("emit maintenanceCompleted(record)")),
                 "Maintenance execution must not emit completion without backend acknowledgement");
    }
};

QTEST_MAIN(WorkflowMaintenanceServiceTest)
#include "workflow_maintenance_service_test.moc"
