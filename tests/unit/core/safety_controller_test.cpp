// SafetyControllerTest — Tests for Safety Controller
//
// Test coverage:
//   - State transition validation (OP→INIT blocked, OP→PREOP blocked, etc.)
//   - Valid state transitions (INIT→OP, PREOP→SAFE-OP, SAFE-OP→OP)
//   - Invalid from/to state handling
//   - SDO write validation (valid, negative position, empty index, read-only objects)
//   - Free Run start requirements (OP state)
//   - SDO write during Free Run blocking
//   - Safety violation signal emission
#include "services/SafetyController.h"
#include <QSignalSpy>
#include <QTest>

class SafetyControllerTest : public QObject {
    Q_OBJECT
private slots:
    // OP to INIT transition should be blocked
    void testOpToInitBlocked() {
        SafetyController sc;
        ValidationResult r = sc.validateStateTransition(8, 1);
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("OP"));
        QVERIFY(r.reason.contains("INIT"));
    }

    // OP to PRE-OP transition should be blocked
    void testOpToPreOpBlocked() {
        SafetyController sc;
        ValidationResult r = sc.validateStateTransition(8, 2);
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("SAFE-OP"));
    }

    // OP to SAFE-OP transition should be allowed
    void testOpToSafeOpAllowed() {
        SafetyController sc;
        ValidationResult r = sc.validateStateTransition(8, 4);
        QCOMPARE(r.allowed, true);
        QVERIFY(r.reason.isEmpty());
    }

    // INIT to OP transition should be allowed
    void testInitToOpAllowed() {
        SafetyController sc;
        ValidationResult r = sc.validateStateTransition(1, 8);
        QCOMPARE(r.allowed, true);
    }

    // PRE-OP to SAFE-OP transition should be allowed
    void testPreOpToSafeOpAllowed() {
        SafetyController sc;
        ValidationResult r = sc.validateStateTransition(2, 4);
        QCOMPARE(r.allowed, true);
    }

    // SAFE-OP to OP transition should be allowed
    void testSafeOpToOpAllowed() {
        SafetyController sc;
        ValidationResult r = sc.validateStateTransition(4, 8);
        QCOMPARE(r.allowed, true);
    }

    // Invalid from state code is rejected
    void testInvalidFromState() {
        SafetyController sc;
        ValidationResult r = sc.validateStateTransition(99, 1);
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("Invalid"));
    }

    // Invalid to state code is rejected
    void testInvalidToState() {
        SafetyController sc;
        ValidationResult r = sc.validateStateTransition(1, 99);
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("Invalid"));
    }

    // Valid SDO write is allowed
    void testSdoWriteValid() {
        SafetyController sc;
        ValidationResult r = sc.validateSdoWrite(0, "6060", "08");
        QCOMPARE(r.allowed, true);
    }

    // SDO write with negative position is rejected
    void testSdoWriteNegativePosition() {
        SafetyController sc;
        ValidationResult r = sc.validateSdoWrite(-1, "6060", "08");
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("position"));
    }

    // SDO write with empty index is rejected
    void testSdoWriteEmptyIndex() {
        SafetyController sc;
        ValidationResult r = sc.validateSdoWrite(0, "", "08");
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("empty"));
    }

    // SDO write to read-only object (0x1000) is rejected
    void testSdoWriteReadOnlyObject() {
        SafetyController sc;
        ValidationResult r = sc.validateSdoWrite(0, "1000", "00");
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("read-only"));
    }

    // SDO write to read-only error register (0x1001) is rejected
    void testSdoWriteReadOnlyErrorRegister() {
        SafetyController sc;
        ValidationResult r = sc.validateSdoWrite(0, "1001", "00");
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("read-only"));
    }

    // SDO write to read-only identity object (0x1018) is rejected
    void testSdoWriteReadOnlyIdentity() {
        SafetyController sc;
        ValidationResult r = sc.validateSdoWrite(0, "1018", "00");
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("read-only"));
    }

    // Free Run start requires OP state
    void testFreeRunStartRequiresOp() {
        SafetyController sc;
        ValidationResult r = sc.validateFreeRunStart(false);
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("OP state"));
    }

    // Free Run start with OP state is allowed
    void testFreeRunStartWithOpAllowed() {
        SafetyController sc;
        ValidationResult r = sc.validateFreeRunStart(true);
        QCOMPARE(r.allowed, true);
    }

    // SDO write during Free Run is blocked
    void testSdoWriteDuringFreeRunBlocked() {
        SafetyController sc;
        ValidationResult r = sc.validateSdoWriteDuringFreeRun(true);
        QCOMPARE(r.allowed, false);
        QVERIFY(r.reason.contains("Free Run"));
    }

    // SDO write without Free Run is allowed
    void testSdoWriteWithoutFreeRunAllowed() {
        SafetyController sc;
        ValidationResult r = sc.validateSdoWriteDuringFreeRun(false);
        QCOMPARE(r.allowed, true);
    }

    // Safety violation signal fires on blocked transition
    void testSafetyViolationSignal() {
        SafetyController sc;
        QSignalSpy spy(&sc, &SafetyController::safetyViolation);
        QVERIFY(spy.isValid());

        sc.validateStateTransition(8, 1);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy[0][0].toString(), "stateTransition");
    }
};

QTEST_MAIN(SafetyControllerTest)
#include "safety_controller_test.moc"
