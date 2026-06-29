// FoEHandlerTest — Tests for FoEHandler path validation
//
// Test coverage:
//   - Absolute path requirement
//   - Path traversal rejection (..)
//   - Directory existence check

#include "handlers/FoEHandler.h"

#include <QCoreApplication>
#include <QJsonObject>

#include <cstdlib>
#include <iostream>

namespace {
int failures = 0;
void fail(const QString &msg) {
    std::cerr << msg.toStdString() << '\n';
    ++failures;
}
void expectTrue(bool cond, const QString &msg) {
    if (!cond) fail(msg);
}
}

void testAbsolutePathRequired() {
    FoEHandler handler;
    QString error;
    bool ok = handler.validateFilePath("relative/path.bin", &error);
    expectTrue(!ok, "T1: relative path rejected");
    expectTrue(!error.isEmpty(), "T1: error message set");
}

void testPathTraversalRejected() {
    FoEHandler handler;
    QString error;
    bool ok = handler.validateFilePath("/tmp/../etc/passwd", &error);
    expectTrue(!ok, "T2: path traversal with /../ rejected");
    expectTrue(error.contains(".."), "T2: error mentions ..");
}

void testPathTraversalEndRejected() {
    FoEHandler handler;
    QString error;
    bool ok = handler.validateFilePath("/tmp/..", &error);
    expectTrue(!ok, "T3: path ending with /.. rejected");
}

void testValidAbsolutePathAccepted() {
    FoEHandler handler;
    QString error;
    // /tmp always exists on Linux
    bool ok = handler.validateFilePath("/tmp/firmware.bin", &error);
    expectTrue(ok, "T4: valid absolute path accepted");
    expectTrue(error.isEmpty(), "T4: no error for valid path");
}

void testNonExistentDirectoryRejected() {
    FoEHandler handler;
    QString error;
    bool ok = handler.validateFilePath("/nonexistent/dir/file.bin", &error);
    expectTrue(!ok, "T5: non-existent directory rejected");
    expectTrue(error.contains("does not exist"), "T5: error mentions missing dir");
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    testAbsolutePathRequired();
    testPathTraversalRejected();
    testPathTraversalEndRejected();
    testValidAbsolutePathAccepted();
    testNonExistentDirectoryRejected();

    if (failures > 0) {
        std::cerr << failures << " test(s) failed.\n";
        return 1;
    }
    std::cout << "All FoE handler tests passed.\n";
    return 0;
}
