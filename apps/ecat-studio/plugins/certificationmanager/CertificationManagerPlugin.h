#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class CertificationManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit CertificationManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct Certificate {
    QString id;
    QString name;
    QString issuer;
    QString standard;
    QDateTime issuedAt;
    QDateTime expiresAt;
    QString status;
    QString serialNumber;
  };

  struct CertificationStatus {
    QString certificateId;
    QString status;
    QDateTime lastChecked;
    QString notes;
  };

  struct RenewalReminder {
    QString certificateId;
    QString certificateName;
    QDateTime expiryDate;
    int daysUntilExpiry;
    bool notified;
  };

  void addCertificate(const Certificate &cert);
  void removeCertificate(int index);
  void updateCertificate(int index, const Certificate &cert);
  int certificateCount() const;

  void validateCertificate(int index);
  bool isExpired(int index) const;
  bool isExpiringSoon(int index, int daysThreshold = 30) const;

  void checkRenewals();
  int renewalCount() const;

  void exportReport(const QString &path);

  QTableWidget *certificateTable() const;
  QTableWidget *statusTable() const;
  QTableWidget *renewalTable() const;
  QLabel *statusLabel() const;

signals:
  void certificateAdded(const QString &certId);
  void certificateExpired(const QString &certId);
  void renewalDue(const QString &certId, int daysLeft);

private:
  void buildUi();
  void rebuildCertificateTable();
  void rebuildStatusTable();
  void rebuildRenewalTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  QTableWidget *certificateTable_ = nullptr;
  QLineEdit *certSearchEdit_ = nullptr;
  QPushButton *addCertBtn_ = nullptr;
  QPushButton *removeCertBtn_ = nullptr;
  QPushButton *editCertBtn_ = nullptr;
  QPushButton *validateBtn_ = nullptr;

  QTableWidget *statusTable_ = nullptr;

  QTableWidget *renewalTable_ = nullptr;
  QPushButton *checkRenewalsBtn_ = nullptr;
  QPushButton *exportReportBtn_ = nullptr;

  QLabel *statusLabel_ = nullptr;

  QVector<Certificate> certificates_;
  QVector<CertificationStatus> statuses_;
  QVector<RenewalReminder> renewals_;
};
