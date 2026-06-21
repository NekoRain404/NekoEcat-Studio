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

class SecurityManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit SecurityManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct User {
    QString id;
    QString username;
    QString role;
    QString email;
    bool active;
    QDateTime createdAt;
    QDateTime lastLogin;
  };

  struct Role {
    QString id;
    QString name;
    QString description;
    QStringList permissions;
  };

  struct Permission {
    QString id;
    QString resource;
    QString action;
    QString description;
  };

  struct AuditEntry {
    QDateTime timestamp;
    QString user;
    QString action;
    QString resource;
    QString details;
    QString severity;
  };

  void addUser(const User &user);
  void removeUser(int index);
  void updateUser(int index, const User &user);
  int userCount() const;

  void addRole(const Role &role);
  void removeRole(int index);
  int roleCount() const;

  void addPermission(const Permission &perm);
  void removePermission(int index);
  int permissionCount() const;

  void addAuditEntry(const AuditEntry &entry);
  int auditCount() const;
  void filterAudit(const QString &user, const QString &severity);

  void exportSecurityReport(const QString &path);

  QTableWidget *userTable() const;
  QTableWidget *roleTable() const;
  QTableWidget *permissionTable() const;
  QTableWidget *auditTable() const;
  QLabel *statusLabel() const;

signals:
  void userAdded(const QString &userId);
  void userRemoved(const QString &userId);
  void auditEntryAdded();

private:
  void buildUi();
  void rebuildUserTable();
  void rebuildRoleTable();
  void rebuildPermissionTable();
  void rebuildAuditTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  QTableWidget *userTable_ = nullptr;
  QLineEdit *userSearchEdit_ = nullptr;
  QPushButton *addUserBtn_ = nullptr;
  QPushButton *removeUserBtn_ = nullptr;
  QPushButton *editUserBtn_ = nullptr;

  QTableWidget *roleTable_ = nullptr;
  QPushButton *addRoleBtn_ = nullptr;
  QPushButton *removeRoleBtn_ = nullptr;

  QTableWidget *permissionTable_ = nullptr;
  QPushButton *addPermBtn_ = nullptr;
  QPushButton *removePermBtn_ = nullptr;

  QTableWidget *auditTable_ = nullptr;
  QComboBox *auditSeverityFilter_ = nullptr;
  QLineEdit *auditUserFilter_ = nullptr;
  QPushButton *filterAuditBtn_ = nullptr;
  QPushButton *exportReportBtn_ = nullptr;

  QLabel *statusLabel_ = nullptr;

  QVector<User> users_;
  QVector<Role> roles_;
  QVector<Permission> permissions_;
  QVector<AuditEntry> auditLog_;
  QVector<int> filteredAudit_;
};
