#include "CertificationManagerPlugin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

CertificationManagerPlugin::CertificationManagerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString CertificationManagerPlugin::id() const {
  return "certificationmanager";
}
QString CertificationManagerPlugin::displayName() const {
  return "Certification Manager";
}
QString CertificationManagerPlugin::displayNameZh() const {
  return "认证管理器";
}
int CertificationManagerPlugin::defaultOrder() const { return 290; }
bool CertificationManagerPlugin::visible() const { return true; }

void CertificationManagerPlugin::activate() {}
void CertificationManagerPlugin::deactivate() {}

QWidget *CertificationManagerPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void CertificationManagerPlugin::addCertificate(const Certificate &cert) {
  certificates_.append(cert);
  rebuildCertificateTable();
  emit certificateAdded(cert.id);
}

void CertificationManagerPlugin::removeCertificate(int index) {
  if (index >= 0 && index < certificates_.size()) {
    certificates_.removeAt(index);
    rebuildCertificateTable();
  }
}

void CertificationManagerPlugin::updateCertificate(int index,
                                                    const Certificate &cert) {
  if (index >= 0 && index < certificates_.size()) {
    certificates_[index] = cert;
    rebuildCertificateTable();
  }
}

int CertificationManagerPlugin::certificateCount() const {
  return certificates_.size();
}

void CertificationManagerPlugin::validateCertificate(int index) {
  if (index < 0 || index >= certificates_.size()) return;
  auto &cert = certificates_[index];
  if (QDateTime::currentDateTime() > cert.expiresAt) {
    cert.status = "Expired";
    emit certificateExpired(cert.id);
  } else if (isExpiringSoon(index)) {
    cert.status = "Expiring Soon";
    int days = QDateTime::currentDateTime().daysTo(cert.expiresAt);
    emit renewalDue(cert.id, days);
  } else {
    cert.status = "Unverified";
  }
  rebuildCertificateTable();
}

bool CertificationManagerPlugin::isExpired(int index) const {
  if (index < 0 || index >= certificates_.size()) return false;
  return QDateTime::currentDateTime() > certificates_[index].expiresAt;
}

bool CertificationManagerPlugin::isExpiringSoon(int index,
                                                 int daysThreshold) const {
  if (index < 0 || index >= certificates_.size()) return false;
  auto now = QDateTime::currentDateTime();
  auto &cert = certificates_[index];
  return now <= cert.expiresAt &&
         now.daysTo(cert.expiresAt) <= daysThreshold;
}

void CertificationManagerPlugin::checkRenewals() {
  renewals_.clear();
  for (int i = 0; i < certificates_.size(); ++i) {
    const auto &cert = certificates_[i];
    int days = QDateTime::currentDateTime().daysTo(cert.expiresAt);
    if (days <= 30) {
      renewals_.append({cert.id, cert.name, cert.expiresAt, days, false});
    }
  }
  rebuildRenewalTable();
}

int CertificationManagerPlugin::renewalCount() const {
  return renewals_.size();
}

void CertificationManagerPlugin::exportReport(const QString &path) {
  QFile f(path);
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&f);
    out << "Certification Report\n";
    out << "====================\n\n";
    out << "Total Certificates: " << certificates_.size() << "\n\n";
    out << "--- Certificates ---\n";
    for (const auto &c : certificates_) {
      out << c.name << " [" << c.standard << "] - " << c.status << "\n";
      out << "  Issuer: " << c.issuer << "\n";
      out << "  Serial: " << c.serialNumber << "\n";
      out << "  Issued: " << c.issuedAt.toString(Qt::ISODate) << "\n";
      out << "  Expires: " << c.expiresAt.toString(Qt::ISODate) << "\n\n";
    }
    out << "--- Renewals Needed ---\n";
    for (const auto &r : renewals_) {
      out << r.certificateName << " - " << r.daysUntilExpiry << " days\n";
    }
  }
}

QTableWidget *CertificationManagerPlugin::certificateTable() const {
  return certificateTable_;
}
QTableWidget *CertificationManagerPlugin::statusTable() const {
  return statusTable_;
}
QTableWidget *CertificationManagerPlugin::renewalTable() const {
  return renewalTable_;
}
QLabel *CertificationManagerPlugin::statusLabel() const {
  return statusLabel_;
}

void CertificationManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  tabs_ = new QTabWidget;

  auto *certTab = new QWidget;
  auto *certLayout = new QVBoxLayout(certTab);
  auto *certSearchRow = new QWidget;
  auto *certSearchLayout = new QHBoxLayout(certSearchRow);
  certSearchEdit_ = new QLineEdit;
  certSearchEdit_->setPlaceholderText("Search certificates...");
  certSearchLayout->addWidget(certSearchEdit_);
  certLayout->addWidget(certSearchRow);

  certificateTable_ = new QTableWidget;
  certificateTable_->setColumnCount(7);
  certificateTable_->setHorizontalHeaderLabels(
      {"ID", "Name", "Issuer", "Standard", "Issued", "Expires", "Status"});
  certLayout->addWidget(certificateTable_);

  auto *certBtnRow = new QWidget;
  auto *certBtnLayout = new QHBoxLayout(certBtnRow);
  addCertBtn_ = new QPushButton("Add Certificate");
  removeCertBtn_ = new QPushButton("Remove");
  editCertBtn_ = new QPushButton("Edit");
  validateBtn_ = new QPushButton("Validate");
  certBtnLayout->addWidget(addCertBtn_);
  certBtnLayout->addWidget(removeCertBtn_);
  certBtnLayout->addWidget(editCertBtn_);
  certBtnLayout->addWidget(validateBtn_);
  certLayout->addWidget(certBtnRow);
  tabs_->addTab(certTab, "Certificates");

  auto *statusTab = new QWidget;
  auto *statusLayout = new QVBoxLayout(statusTab);
  statusTable_ = new QTableWidget;
  statusTable_->setColumnCount(4);
  statusTable_->setHorizontalHeaderLabels(
      {"Certificate", "Status", "Last Checked", "Notes"});
  statusLayout->addWidget(statusTable_);
  tabs_->addTab(statusTab, "Status");

  auto *renewalTab = new QWidget;
  auto *renewalLayout = new QVBoxLayout(renewalTab);
  renewalTable_ = new QTableWidget;
  renewalTable_->setColumnCount(4);
  renewalTable_->setHorizontalHeaderLabels(
      {"Certificate", "Expiry Date", "Days Left", "Notified"});
  renewalLayout->addWidget(renewalTable_);

  auto *renewalBtnRow = new QWidget;
  auto *renewalBtnLayout = new QHBoxLayout(renewalBtnRow);
  checkRenewalsBtn_ = new QPushButton("Check Renewals");
  exportReportBtn_ = new QPushButton("Export Report");
  renewalBtnLayout->addWidget(checkRenewalsBtn_);
  renewalBtnLayout->addWidget(exportReportBtn_);
  renewalLayout->addWidget(renewalBtnRow);
  tabs_->addTab(renewalTab, "Renewals");

  mainLayout->addWidget(tabs_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  rebuildCertificateTable();
  rebuildStatusTable();
  rebuildRenewalTable();

  connect(addCertBtn_, &QPushButton::clicked, this, [this]() {
    Certificate c;
    c.id = "cert" + QString::number(certificates_.size() + 1);
    c.name = "New Certificate";
    c.issuer = "";
    c.standard = "";
    c.issuedAt = QDateTime::currentDateTime();
    c.status = "Unverified";
    c.serialNumber = "";
    addCertificate(c);
  });
  connect(removeCertBtn_, &QPushButton::clicked, this, [this]() {
    int row = certificateTable_->currentRow();
    if (row >= 0) removeCertificate(row);
  });
  connect(validateBtn_, &QPushButton::clicked, this, [this]() {
    int row = certificateTable_->currentRow();
    if (row >= 0) validateCertificate(row);
  });
  connect(checkRenewalsBtn_, &QPushButton::clicked, this,
          [this]() { checkRenewals(); });
  connect(exportReportBtn_, &QPushButton::clicked, this, [this]() {
    exportReport("/tmp/certification_report.txt");
  });
}

void CertificationManagerPlugin::rebuildCertificateTable() {
  if (!certificateTable_) return;
  certificateTable_->setRowCount(certificates_.size());
  for (int i = 0; i < certificates_.size(); ++i) {
    const auto &c = certificates_[i];
    certificateTable_->setItem(i, 0, new QTableWidgetItem(c.id));
    certificateTable_->setItem(i, 1, new QTableWidgetItem(c.name));
    certificateTable_->setItem(i, 2, new QTableWidgetItem(c.issuer));
    certificateTable_->setItem(i, 3, new QTableWidgetItem(c.standard));
    certificateTable_->setItem(
        i, 4, new QTableWidgetItem(c.issuedAt.toString(Qt::ISODate)));
    certificateTable_->setItem(
        i, 5, new QTableWidgetItem(c.expiresAt.toString(Qt::ISODate)));
    certificateTable_->setItem(i, 6, new QTableWidgetItem(c.status));
  }
}

void CertificationManagerPlugin::rebuildStatusTable() {
  if (!statusTable_) return;
  statusTable_->setRowCount(statuses_.size());
  for (int i = 0; i < statuses_.size(); ++i) {
    const auto &s = statuses_[i];
    statusTable_->setItem(i, 0, new QTableWidgetItem(s.certificateId));
    statusTable_->setItem(i, 1, new QTableWidgetItem(s.status));
    statusTable_->setItem(
        i, 2, new QTableWidgetItem(s.lastChecked.toString(Qt::ISODate)));
    statusTable_->setItem(i, 3, new QTableWidgetItem(s.notes));
  }
}

void CertificationManagerPlugin::rebuildRenewalTable() {
  if (!renewalTable_) return;
  renewalTable_->setRowCount(renewals_.size());
  for (int i = 0; i < renewals_.size(); ++i) {
    const auto &r = renewals_[i];
    renewalTable_->setItem(i, 0, new QTableWidgetItem(r.certificateName));
    renewalTable_->setItem(
        i, 1, new QTableWidgetItem(r.expiryDate.toString(Qt::ISODate)));
    renewalTable_->setItem(
        i, 2, new QTableWidgetItem(QString::number(r.daysUntilExpiry)));
    renewalTable_->setItem(i, 3,
                           new QTableWidgetItem(r.notified ? "Yes" : "No"));
  }
  if (statusLabel_)
    statusLabel_->setText(
        QString("Certificates: %1 | Renewals: %2")
            .arg(certificates_.size())
            .arg(renewals_.size()));
}
