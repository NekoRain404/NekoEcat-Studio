#include "TopologyChangeModel.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
  }
}

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(expected)
             .arg(actual));
  }
}

void expectEqual(const QString &actual, const QString &expected,
                 const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected '%2', got '%3'").arg(message, expected, actual));
  }
}

void expectChangeKind(TopologyChangeKind actual, TopologyChangeKind expected,
                      const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

SlaveInfo slave(int position, QString name, QString state = QString(),
                QString flags = QString()) {
  SlaveInfo info;
  info.position = position;
  info.name = name;
  info.state = state;
  info.flags = flags;
  return info;
}

void testInitialScanDoesNotReportChanges() {
  const auto changes = detectTopologyChanges({}, {slave(0, "Drive", "OP")});
  expectTrue(changes.isEmpty(), "initial scan does not report changes");
}

void testTopologyChangeOrderAndPayloads() {
  const QVector<SlaveInfo> previous = {
      slave(0, "Drive A", "PREOP", "+"),
      slave(1, "Drive B", "OP", "-"),
      slave(2, "Terminal", "OP", "+"),
  };
  const QVector<SlaveInfo> current = {
      slave(0, "Drive A2", "OP", "-"),
      slave(2, "Terminal", "OP", "+"),
      slave(3, "Extra", "PREOP", "+"),
  };

  const auto changes = detectTopologyChanges(previous, current);
  expectEqual(changes.size(), 5,
              "topology change detector reports all changes");
  expectChangeKind(changes.at(0).kind, TopologyChangeKind::NameChanged,
                   "name change is reported during current pass");
  expectEqual(changes.at(0).position, 0, "name change keeps slave position");
  expectEqual(changes.at(0).previous.name, "Drive A",
              "name change keeps previous name");
  expectEqual(changes.at(0).current.name, "Drive A2",
              "name change keeps current name");
  expectChangeKind(changes.at(1).kind, TopologyChangeKind::StateChanged,
                   "state change follows name change for same slave");
  expectEqual(changes.at(1).previous.state, "PREOP",
              "state change keeps previous state");
  expectEqual(changes.at(1).current.state, "OP",
              "state change keeps current state");
  expectChangeKind(changes.at(2).kind, TopologyChangeKind::FlagsChanged,
                   "flags change follows state change for same slave");
  expectChangeKind(changes.at(3).kind, TopologyChangeKind::Added,
                   "added slave is reported in current pass");
  expectEqual(changes.at(3).current.name, "Extra",
              "added slave keeps current data");
  expectChangeKind(changes.at(4).kind, TopologyChangeKind::Removed,
                   "removed slave is reported after current pass");
  expectEqual(changes.at(4).previous.name, "Drive B",
              "removed slave keeps previous data");
}

void testIdenticalTopologyHasNoChanges() {
  const QVector<SlaveInfo> previous = {slave(0, "Drive", "OP", "+")};
  const QVector<SlaveInfo> current = {slave(0, "Drive", "OP", "+")};
  const auto changes = detectTopologyChanges(previous, current);
  expectTrue(changes.isEmpty(), "identical topology has no changes");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testInitialScanDoesNotReportChanges();
  testTopologyChangeOrderAndPayloads();
  testIdenticalTopologyHasNoChanges();
  return 0;
}
