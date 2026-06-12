#include "models/TopologyBaselineModel.h"

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

void expectIssueKind(TopologyBaselineIssueKind actual,
                     TopologyBaselineIssueKind expected,
                     const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

SlaveInfo slave(int position, QString name, QString state = QString(),
                QString rawLine = QString()) {
  SlaveInfo info;
  info.position = position;
  info.name = name;
  info.state = state;
  info.rawLine = rawLine;
  return info;
}

void testEmptyBaselineHasNoIssues() {
  const auto issues = compareTopologyBaseline({}, {slave(0, "Drive", "OP")});
  expectTrue(issues.isEmpty(), "empty topology baseline has no issues");
}

void testMissingNameStateAndUnexpectedIssues() {
  const QVector<SlaveInfo> baseline = {
      slave(0, "Drive A", "OP"),
      slave(1, "Drive B", "PREOP"),
      slave(2, "Terminal", "OP"),
  };
  const QVector<SlaveInfo> current = {
      slave(0, "Drive A2", "SAFEOP"),
      slave(2, "Terminal", "OP"),
      slave(3, "Extra", "PREOP"),
  };

  const auto issues = compareTopologyBaseline(baseline, current);
  expectEqual(issues.size(), 4, "topology comparison reports all issues");
  expectIssueKind(issues.at(0).kind, TopologyBaselineIssueKind::NameChanged,
                  "name change is reported in baseline order");
  expectEqual(issues.at(0).position, 0, "name change keeps slave position");
  expectEqual(issues.at(0).baseline.name, "Drive A",
              "name change keeps baseline name");
  expectEqual(issues.at(0).current.name, "Drive A2",
              "name change keeps current name");
  expectIssueKind(issues.at(1).kind, TopologyBaselineIssueKind::StateChanged,
                  "state change follows name change for the same slave");
  expectIssueKind(issues.at(2).kind, TopologyBaselineIssueKind::MissingSlave,
                  "missing baseline slave is reported");
  expectEqual(issues.at(2).position, 1,
              "missing slave keeps baseline position");
  expectIssueKind(issues.at(3).kind, TopologyBaselineIssueKind::UnexpectedSlave,
                  "unexpected slave is reported after baseline pass");
  expectEqual(issues.at(3).current.name, "Extra",
              "unexpected slave keeps current slave data");
}

void testEmptyStateDoesNotCreateStateIssue() {
  const QVector<SlaveInfo> baseline = {slave(0, "Drive", QString())};
  const QVector<SlaveInfo> current = {slave(0, "Drive", "OP")};
  const auto issues = compareTopologyBaseline(baseline, current);
  expectTrue(issues.isEmpty(), "empty baseline state does not create issue");
}

void testTopologyDisplayNameFallback() {
  expectEqual(topologySlaveDisplayName(slave(1, "", "OP", "1  OP  Raw")),
              "1  OP  Raw", "empty slave name falls back to raw line");
  expectEqual(topologySlaveDisplayName(slave(1, " ", "OP", "raw")), " ",
              "blank but non-empty slave name preserves previous display");
  expectEqual(topologySlaveDisplayName(slave(1, "Drive", "OP", "raw")), "Drive",
              "non-empty slave name is preferred");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyBaselineHasNoIssues();
  testMissingNameStateAndUnexpectedIssues();
  testEmptyStateDoesNotCreateStateIssue();
  testTopologyDisplayNameFallback();
  return 0;
}
