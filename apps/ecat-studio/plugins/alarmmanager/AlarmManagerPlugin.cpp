#include "AlarmManagerPlugin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

AlarmManagerPlugin::AlarmManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString AlarmManagerPlugin::id() const { return "alarmmanager"; }
QString AlarmManagerPlugin::displayName() const { return "Alarm Manager"; }
QString AlarmManagerPlugin::displayNameZh() const { return "告警管理器"; }
int AlarmManagerPlugin::defaultOrder() const { return 240; }
bool AlarmManagerPlugin::visible() const { return false; }

void AlarmManagerPlugin::activate() {}
void AlarmManagerPlugin::deactivate() {}

QWidget *AlarmManagerPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void AlarmManagerPlugin::addRule(const AlarmRule &rule) {
  rules_.append(rule);
  rebuildRulesTable();
}

void AlarmManagerPlugin::removeRule(int index) {
  if (index >= 0 && index < rules_.size()) {
    rules_.removeAt(index);
    rebuildRulesTable();
  }
}

int AlarmManagerPlugin::ruleCount() const { return rules_.size(); }

void AlarmManagerPlugin::addRecord(const AlarmRecord &record) {
  records_.append(record);
  filteredRecords_ = records_;
  rebuildHistoryTable();
  emit alarmRaised(record.ruleId, record.message);
}

void AlarmManagerPlugin::removeRecord(int index) {
  if (index >= 0 && index < records_.size()) {
    records_.removeAt(index);
    filteredRecords_ = records_;
    rebuildHistoryTable();
  }
}

int AlarmManagerPlugin::recordCount() const { return records_.size(); }

void AlarmManagerPlugin::acknowledgeRecord(int index) {
  if (index >= 0 && index < filteredRecords_.size()) {
    auto &rec = filteredRecords_[index];
    for (auto &r : records_) {
      if (r.timestamp == rec.timestamp && r.ruleId == rec.ruleId) {
        r.acknowledged = true;
        break;
      }
    }
    rec.acknowledged = true;
    rebuildHistoryTable();
    emit alarmAcknowledged(index);
  }
}

void AlarmManagerPlugin::filterRecords(const QString &query) {
  if (query.isEmpty()) {
    filteredRecords_ = records_;
  } else {
    filteredRecords_.clear();
    for (const auto &r : records_) {
      if (r.message.contains(query, Qt::CaseInsensitive) ||
          r.severity.contains(query, Qt::CaseInsensitive) ||
          r.channel.contains(query, Qt::CaseInsensitive)) {
        filteredRecords_.append(r);
      }
    }
  }
  rebuildHistoryTable();
  if (statusLabel_) statusLabel_->setText(tr("%1 records").arg(filteredRecords_.size()));
}

void AlarmManagerPlugin::addChannel(const QString &channel) {
  channels_.append(channel);
  rebuildChannelsTable();
}

void AlarmManagerPlugin::removeChannel(int index) {
  if (index >= 0 && index < channels_.size()) {
    channels_.removeAt(index);
    rebuildChannelsTable();
  }
}

int AlarmManagerPlugin::channelCount() const { return channels_.size(); }

void AlarmManagerPlugin::addEscalationPolicy(const EscalationPolicy &policy) {
  policies_.append(policy);
  rebuildEscalationTable();
}

void AlarmManagerPlugin::removeEscalationPolicy(int index) {
  if (index >= 0 && index < policies_.size()) {
    policies_.removeAt(index);
    rebuildEscalationTable();
  }
}

int AlarmManagerPlugin::escalationPolicyCount() const { return policies_.size(); }

QString AlarmManagerPlugin::exportAlarmData() const {
  QJsonObject root;
  QJsonArray rulesArr;
  for (const auto &r : rules_) {
    QJsonObject obj;
    obj["id"] = r.id;
    obj["name"] = r.name;
    obj["condition"] = r.condition;
    obj["severity"] = r.severity;
    obj["enabled"] = r.enabled;
    obj["createdAt"] = r.createdAt.toString(Qt::ISODate);
    rulesArr.append(obj);
  }
  root["rules"] = rulesArr;

  QJsonArray recordsArr;
  for (const auto &r : records_) {
    QJsonObject obj;
    obj["ruleId"] = r.ruleId;
    obj["message"] = r.message;
    obj["severity"] = r.severity;
    obj["channel"] = r.channel;
    obj["timestamp"] = r.timestamp.toString(Qt::ISODate);
    obj["acknowledged"] = r.acknowledged;
    recordsArr.append(obj);
  }
  root["records"] = recordsArr;

  QJsonArray policiesArr;
  for (const auto &p : policies_) {
    QJsonObject obj;
    obj["id"] = p.id;
    obj["name"] = p.name;
    obj["delaySeconds"] = p.delaySeconds;
    obj["targetChannel"] = p.targetChannel;
    obj["severity"] = p.severity;
    policiesArr.append(obj);
  }
  root["policies"] = policiesArr;

  return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QTabWidget *AlarmManagerPlugin::tabs() const { return tabs_; }
QTableWidget *AlarmManagerPlugin::rulesTable() const { return rulesTable_; }
QTableWidget *AlarmManagerPlugin::historyTable() const { return historyTable_; }
QTableWidget *AlarmManagerPlugin::channelsTable() const { return channelsTable_; }
QTableWidget *AlarmManagerPlugin::escalationTable() const { return escalationTable_; }
QLabel *AlarmManagerPlugin::statusLabel() const { return statusLabel_; }

void AlarmManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  tabs_ = new QTabWidget;

  auto *rulesTab = new QWidget;
  auto *rulesLayout = new QVBoxLayout(rulesTab);
  rulesTable_ = new QTableWidget;
  rulesTable_->setColumnCount(4);
  rulesTable_->setHorizontalHeaderLabels({"Name", "Condition", "Severity", "Enabled"});
  rulesTable_->horizontalHeader()->setStretchLastSection(true);
  rulesLayout->addWidget(rulesTable_);
  auto *rulesBtnRow = new QWidget;
  auto *rulesBtnLayout = new QHBoxLayout(rulesBtnRow);
  addRuleBtn_ = new QPushButton("Add");
  removeRuleBtn_ = new QPushButton("Remove");
  toggleRuleBtn_ = new QPushButton("Toggle");
  rulesBtnLayout->addWidget(addRuleBtn_);
  rulesBtnLayout->addWidget(removeRuleBtn_);
  rulesBtnLayout->addWidget(toggleRuleBtn_);
  rulesLayout->addWidget(rulesBtnRow);
  tabs_->addTab(rulesTab, "Alarm Rules");

  auto *historyTab = new QWidget;
  auto *historyLayout = new QVBoxLayout(historyTab);
  auto *filterRow = new QWidget;
  auto *filterLayout = new QHBoxLayout(filterRow);
  filterEdit_ = new QLineEdit;
  filterEdit_->setPlaceholderText("Filter...");
  filterLayout->addWidget(filterEdit_);
  historyLayout->addWidget(filterRow);
  historyTable_ = new QTableWidget;
  historyTable_->setColumnCount(5);
  historyTable_->setHorizontalHeaderLabels({"Time", "Severity", "Message", "Channel", "Acknowledged"});
  historyTable_->horizontalHeader()->setStretchLastSection(true);
  historyLayout->addWidget(historyTable_);
  ackBtn_ = new QPushButton("Acknowledge");
  historyLayout->addWidget(ackBtn_);
  tabs_->addTab(historyTab, "Alarm History");

  auto *channelsTab = new QWidget;
  auto *channelsLayout = new QVBoxLayout(channelsTab);
  channelsTable_ = new QTableWidget;
  channelsTable_->setColumnCount(2);
  channelsTable_->setHorizontalHeaderLabels({"Channel", "Status"});
  channelsTable_->horizontalHeader()->setStretchLastSection(true);
  channelsLayout->addWidget(channelsTable_);
  auto *channelsBtnRow = new QWidget;
  auto *channelsBtnLayout = new QHBoxLayout(channelsBtnRow);
  addChannelBtn_ = new QPushButton("Add");
  removeChannelBtn_ = new QPushButton("Remove");
  channelsBtnLayout->addWidget(addChannelBtn_);
  channelsBtnLayout->addWidget(removeChannelBtn_);
  channelsLayout->addWidget(channelsBtnRow);
  tabs_->addTab(channelsTab, "Notifications");

  auto *escalationTab = new QWidget;
  auto *escalationLayout = new QVBoxLayout(escalationTab);
  escalationTable_ = new QTableWidget;
  escalationTable_->setColumnCount(4);
  escalationTable_->setHorizontalHeaderLabels({"Name", "Delay", "Target", "Severity"});
  escalationTable_->horizontalHeader()->setStretchLastSection(true);
  escalationLayout->addWidget(escalationTable_);
  auto *escalationBtnRow = new QWidget;
  auto *escalationBtnLayout = new QHBoxLayout(escalationBtnRow);
  addEscalationBtn_ = new QPushButton("Add");
  removeEscalationBtn_ = new QPushButton("Remove");
  escalationBtnLayout->addWidget(addEscalationBtn_);
  escalationBtnLayout->addWidget(removeEscalationBtn_);
  escalationLayout->addWidget(escalationBtnRow);
  tabs_->addTab(escalationTab, "Escalation");

  mainLayout->addWidget(tabs_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  connect(addRuleBtn_, &QPushButton::clicked, this, [this]() {
    AlarmRule r;
    r.id = "rule_" + QString::number(rules_.size());
    r.name = "New Rule";
    r.condition = "";
    r.severity = "warning";
    r.enabled = true;
    r.createdAt = QDateTime::currentDateTime();
    addRule(r);
  });
  connect(removeRuleBtn_, &QPushButton::clicked, this, [this]() {
    int row = rulesTable_->currentRow();
    if (row >= 0) removeRule(row);
  });
  connect(toggleRuleBtn_, &QPushButton::clicked, this, [this]() {
    int row = rulesTable_->currentRow();
    if (row >= 0 && row < rules_.size()) {
      rules_[row].enabled = !rules_[row].enabled;
      rebuildRulesTable();
    }
  });
  connect(filterEdit_, &QLineEdit::textChanged, this, [this](const QString &text) {
    filterRecords(text);
  });
  connect(ackBtn_, &QPushButton::clicked, this, [this]() {
    int row = historyTable_->currentRow();
    if (row >= 0) acknowledgeRecord(row);
  });
  connect(addChannelBtn_, &QPushButton::clicked, this, [this]() {
    addChannel("channel_" + QString::number(channels_.size()));
  });
  connect(removeChannelBtn_, &QPushButton::clicked, this, [this]() {
    int row = channelsTable_->currentRow();
    if (row >= 0) removeChannel(row);
  });
  connect(addEscalationBtn_, &QPushButton::clicked, this, [this]() {
    EscalationPolicy p;
    p.id = "esc_" + QString::number(policies_.size());
    p.name = "New Policy";
    p.delaySeconds = 300;
    p.targetChannel = "email";
    p.severity = "critical";
    addEscalationPolicy(p);
  });
  connect(removeEscalationBtn_, &QPushButton::clicked, this, [this]() {
    int row = escalationTable_->currentRow();
    if (row >= 0) removeEscalationPolicy(row);
  });
}

void AlarmManagerPlugin::rebuildRulesTable() {
  if (!rulesTable_) return;
  rulesTable_->setRowCount(rules_.size());
  for (int i = 0; i < rules_.size(); ++i) {
    const auto &r = rules_[i];
    rulesTable_->setItem(i, 0, new QTableWidgetItem(r.name));
    rulesTable_->setItem(i, 1, new QTableWidgetItem(r.condition));
    rulesTable_->setItem(i, 2, new QTableWidgetItem(r.severity));
    rulesTable_->setItem(i, 3, new QTableWidgetItem(r.enabled ? "Yes" : "No"));
  }
}

void AlarmManagerPlugin::rebuildHistoryTable() {
  if (!historyTable_) return;
  historyTable_->setRowCount(filteredRecords_.size());
  for (int i = 0; i < filteredRecords_.size(); ++i) {
    const auto &r = filteredRecords_[i];
    historyTable_->setItem(i, 0, new QTableWidgetItem(r.timestamp.toString(Qt::ISODate)));
    historyTable_->setItem(i, 1, new QTableWidgetItem(r.severity));
    historyTable_->setItem(i, 2, new QTableWidgetItem(r.message));
    historyTable_->setItem(i, 3, new QTableWidgetItem(r.channel));
    historyTable_->setItem(i, 4, new QTableWidgetItem(r.acknowledged ? "Yes" : "No"));
  }
}

void AlarmManagerPlugin::rebuildChannelsTable() {
  if (!channelsTable_) return;
  channelsTable_->setRowCount(channels_.size());
  for (int i = 0; i < channels_.size(); ++i) {
    channelsTable_->setItem(i, 0, new QTableWidgetItem(channels_[i]));
    channelsTable_->setItem(i, 1, new QTableWidgetItem("Active"));
  }
}

void AlarmManagerPlugin::rebuildEscalationTable() {
  if (!escalationTable_) return;
  escalationTable_->setRowCount(policies_.size());
  for (int i = 0; i < policies_.size(); ++i) {
    const auto &p = policies_[i];
    escalationTable_->setItem(i, 0, new QTableWidgetItem(p.name));
    escalationTable_->setItem(i, 1, new QTableWidgetItem(QString::number(p.delaySeconds)));
    escalationTable_->setItem(i, 2, new QTableWidgetItem(p.targetChannel));
    escalationTable_->setItem(i, 3, new QTableWidgetItem(p.severity));
  }
}
