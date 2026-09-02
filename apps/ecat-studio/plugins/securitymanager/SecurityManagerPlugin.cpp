#include "SecurityManagerPlugin.h"
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

SecurityManagerPlugin::SecurityManagerPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    auto now = QDateTime::currentDateTime();
    users_ = {
        {"u1", "admin", "Administrator", "admin@ecat.local", true, now, now},
        {"u2", "engineer", "Engineer", "engineer@ecat.local", true, now, now},
        {"u3", "viewer", "Viewer", "viewer@ecat.local", true, now, now},
    };
    roles_ = {
        {"r1", "Administrator", "Full system access", {"read", "write", "delete", "configure", "audit"}},
        {"r2", "Engineer", "Engineering access", {"read", "write", "configure"}},
        {"r3", "Viewer", "Read-only access", {"read"}},
    };
    permissions_ = {
        {"p1", "network", "read", "View network topology"},
        {"p2", "network", "write", "Modify network configuration"},
        {"p3", "network", "configure", "Configure network parameters"},
        {"p4", "sdo", "read", "Read SDO values"},
        {"p5", "sdo", "write", "Write SDO values"},
        {"p6", "system", "delete", "Delete system resources"},
        {"p7", "audit", "audit", "Access audit logs"},
    };
    auditLog_ = {
        {now, "admin", "login", "system", "Successful login", "info"},
        {now, "engineer", "sdo_write", "slave_01", "Wrote SDO 0x1600:01", "info"},
    };
    filteredAudit_ = {0, 1};
    buildUi();
}

QString SecurityManagerPlugin::id() const {
    return "securitymanager";
}
QString SecurityManagerPlugin::displayName() const {
    return "Security Manager";
}
QString SecurityManagerPlugin::displayNameZh() const {
    return "安全管理器";
}
int SecurityManagerPlugin::defaultOrder() const {
    return 280;
}
bool SecurityManagerPlugin::visible() const {
    return false;
}

void SecurityManagerPlugin::activate() {}
void SecurityManagerPlugin::deactivate() {}

QWidget* SecurityManagerPlugin::widget() {
    if (!containerWidget_)
        buildUi();
    return containerWidget_;
}

void SecurityManagerPlugin::addUser(const User& user) {
    users_.append(user);
    rebuildUserTable();
    emit userAdded(user.id);
}

void SecurityManagerPlugin::removeUser(int index) {
    if (index >= 0 && index < users_.size()) {
        QString uid = users_[index].id;
        users_.removeAt(index);
        rebuildUserTable();
        emit userRemoved(uid);
    }
}

void SecurityManagerPlugin::updateUser(int index, const User& user) {
    if (index >= 0 && index < users_.size()) {
        users_[index] = user;
        rebuildUserTable();
    }
}

int SecurityManagerPlugin::userCount() const {
    return users_.size();
}

void SecurityManagerPlugin::addRole(const Role& role) {
    roles_.append(role);
    rebuildRoleTable();
}

void SecurityManagerPlugin::removeRole(int index) {
    if (index >= 0 && index < roles_.size()) {
        roles_.removeAt(index);
        rebuildRoleTable();
    }
}

int SecurityManagerPlugin::roleCount() const {
    return roles_.size();
}

void SecurityManagerPlugin::addPermission(const Permission& perm) {
    permissions_.append(perm);
    rebuildPermissionTable();
}

void SecurityManagerPlugin::removePermission(int index) {
    if (index >= 0 && index < permissions_.size()) {
        permissions_.removeAt(index);
        rebuildPermissionTable();
    }
}

int SecurityManagerPlugin::permissionCount() const {
    return permissions_.size();
}

void SecurityManagerPlugin::addAuditEntry(const AuditEntry& entry) {
    auditLog_.append(entry);
    filteredAudit_.append(auditLog_.size() - 1);
    rebuildAuditTable();
    emit auditEntryAdded();
}

int SecurityManagerPlugin::auditCount() const {
    return auditLog_.size();
}

void SecurityManagerPlugin::filterAudit(const QString& user, const QString& severity) {
    filteredAudit_.clear();
    for (int i = 0; i < auditLog_.size(); ++i) {
        const auto& e = auditLog_[i];
        bool matchUser = user.isEmpty() || e.user.contains(user, Qt::CaseInsensitive);
        bool matchSev = severity.isEmpty() || severity == "All" || e.severity == severity;
        if (matchUser && matchSev)
            filteredAudit_.append(i);
    }
    rebuildAuditTable();
}

bool SecurityManagerPlugin::exportSecurityReport(const QString& path) {
    if (path.isEmpty())
        return false;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&f);
    out << "Security Report\n";
    out << "===============\n\n";
    out << "Users: " << users_.size() << "\n";
    out << "Roles: " << roles_.size() << "\n";
    out << "Permissions: " << permissions_.size() << "\n";
    out << "Audit Entries: " << auditLog_.size() << "\n\n";
    out << "--- Users ---\n";
    for (const auto& u : users_) {
        out << u.username << " [" << u.role << "] " << (u.active ? "active" : "inactive") << "\n";
    }
    out << "\n--- Audit Log ---\n";
    for (const auto& e : auditLog_) {
        out << e.timestamp.toString(Qt::ISODate) << " " << e.user << " " << e.action << " " << e.resource << " "
            << e.severity << "\n";
    }
    return out.status() == QTextStream::Ok && f.flush();
}

QTableWidget* SecurityManagerPlugin::userTable() const {
    return userTable_;
}
QTableWidget* SecurityManagerPlugin::roleTable() const {
    return roleTable_;
}
QTableWidget* SecurityManagerPlugin::permissionTable() const {
    return permissionTable_;
}
QTableWidget* SecurityManagerPlugin::auditTable() const {
    return auditTable_;
}
QLabel* SecurityManagerPlugin::statusLabel() const {
    return statusLabel_;
}

void SecurityManagerPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);
    tabs_ = new QTabWidget;

    auto* userTab = new QWidget;
    auto* userLayout = new QVBoxLayout(userTab);
    auto* userSearchRow = new QWidget;
    auto* userSearchLayout = new QHBoxLayout(userSearchRow);
    userSearchEdit_ = new QLineEdit;
    userSearchEdit_->setPlaceholderText("Search users...");
    userSearchLayout->addWidget(userSearchEdit_);
    userLayout->addWidget(userSearchRow);

    userTable_ = new QTableWidget;
    userTable_->setColumnCount(6);
    userTable_->setHorizontalHeaderLabels({"ID", "Username", "Role", "Email", "Active", "Last Login"});
    userLayout->addWidget(userTable_);

    auto* userBtnRow = new QWidget;
    auto* userBtnLayout = new QHBoxLayout(userBtnRow);
    addUserBtn_ = new QPushButton("Add User");
    removeUserBtn_ = new QPushButton("Remove User");
    editUserBtn_ = new QPushButton("Edit User");
    userBtnLayout->addWidget(addUserBtn_);
    userBtnLayout->addWidget(removeUserBtn_);
    userBtnLayout->addWidget(editUserBtn_);
    userLayout->addWidget(userBtnRow);
    tabs_->addTab(userTab, "Users");

    auto* roleTab = new QWidget;
    auto* roleLayout = new QVBoxLayout(roleTab);
    roleTable_ = new QTableWidget;
    roleTable_->setColumnCount(4);
    roleTable_->setHorizontalHeaderLabels({"ID", "Name", "Description", "Permissions"});
    roleLayout->addWidget(roleTable_);

    auto* roleBtnRow = new QWidget;
    auto* roleBtnLayout = new QHBoxLayout(roleBtnRow);
    addRoleBtn_ = new QPushButton("Add Role");
    removeRoleBtn_ = new QPushButton("Remove Role");
    roleBtnLayout->addWidget(addRoleBtn_);
    roleBtnLayout->addWidget(removeRoleBtn_);
    roleLayout->addWidget(roleBtnRow);
    tabs_->addTab(roleTab, "Roles");

    auto* permTab = new QWidget;
    auto* permLayout = new QVBoxLayout(permTab);
    permissionTable_ = new QTableWidget;
    permissionTable_->setColumnCount(4);
    permissionTable_->setHorizontalHeaderLabels({"ID", "Resource", "Action", "Description"});
    permLayout->addWidget(permissionTable_);

    auto* permBtnRow = new QWidget;
    auto* permBtnLayout = new QHBoxLayout(permBtnRow);
    addPermBtn_ = new QPushButton("Add Permission");
    removePermBtn_ = new QPushButton("Remove Permission");
    permBtnLayout->addWidget(addPermBtn_);
    permBtnLayout->addWidget(removePermBtn_);
    permLayout->addWidget(permBtnRow);
    tabs_->addTab(permTab, "Permissions");

    auto* auditTab = new QWidget;
    auto* auditLayout = new QVBoxLayout(auditTab);
    auto* auditFilterRow = new QWidget;
    auto* auditFilterLayout = new QHBoxLayout(auditFilterRow);
    auditUserFilter_ = new QLineEdit;
    auditUserFilter_->setPlaceholderText("Filter by user...");
    auditSeverityFilter_ = new QComboBox;
    auditSeverityFilter_->addItems({"All", "info", "warning", "critical"});
    filterAuditBtn_ = new QPushButton("Filter");
    auditFilterLayout->addWidget(auditUserFilter_);
    auditFilterLayout->addWidget(auditSeverityFilter_);
    auditFilterLayout->addWidget(filterAuditBtn_);
    auditLayout->addWidget(auditFilterRow);

    auditTable_ = new QTableWidget;
    auditTable_->setColumnCount(6);
    auditTable_->setHorizontalHeaderLabels({"Timestamp", "User", "Action", "Resource", "Details", "Severity"});
    auditLayout->addWidget(auditTable_);

    exportReportBtn_ = new QPushButton("Export Security Report");
    auditLayout->addWidget(exportReportBtn_);
    tabs_->addTab(auditTab, "Audit Log");

    mainLayout->addWidget(tabs_);

    statusLabel_ = new QLabel("Ready");
    mainLayout->addWidget(statusLabel_);

    rebuildUserTable();
    rebuildRoleTable();
    rebuildPermissionTable();
    rebuildAuditTable();

    connect(addUserBtn_, &QPushButton::clicked, this, [this]() {
        User u;
        u.id = "u" + QString::number(users_.size() + 1);
        u.username = "new_user";
        u.role = "Viewer";
        u.email = "";
        u.active = true;
        u.createdAt = QDateTime::currentDateTime();
        u.lastLogin = u.createdAt;
        addUser(u);
    });
    connect(removeUserBtn_, &QPushButton::clicked, this, [this]() {
        int row = userTable_->currentRow();
        if (row >= 0)
            removeUser(row);
    });
    connect(addRoleBtn_, &QPushButton::clicked, this, [this]() {
        Role r;
        r.id = "r" + QString::number(roles_.size() + 1);
        r.name = "New Role";
        r.description = "";
        r.permissions = {"read"};
        addRole(r);
    });
    connect(removeRoleBtn_, &QPushButton::clicked, this, [this]() {
        int row = roleTable_->currentRow();
        if (row >= 0)
            removeRole(row);
    });
    connect(addPermBtn_, &QPushButton::clicked, this, [this]() {
        Permission p;
        p.id = "p" + QString::number(permissions_.size() + 1);
        p.resource = "resource";
        p.action = "read";
        p.description = "";
        addPermission(p);
    });
    connect(removePermBtn_, &QPushButton::clicked, this, [this]() {
        int row = permissionTable_->currentRow();
        if (row >= 0)
            removePermission(row);
    });
    connect(filterAuditBtn_, &QPushButton::clicked, this,
            [this]() { filterAudit(auditUserFilter_->text(), auditSeverityFilter_->currentText()); });
    connect(exportReportBtn_, &QPushButton::clicked, this,
            [this]() { exportSecurityReport("/tmp/security_report.txt"); });
}

void SecurityManagerPlugin::rebuildUserTable() {
    if (!userTable_)
        return;
    userTable_->setRowCount(users_.size());
    for (int i = 0; i < users_.size(); ++i) {
        const auto& u = users_[i];
        userTable_->setItem(i, 0, new QTableWidgetItem(u.id));
        userTable_->setItem(i, 1, new QTableWidgetItem(u.username));
        userTable_->setItem(i, 2, new QTableWidgetItem(u.role));
        userTable_->setItem(i, 3, new QTableWidgetItem(u.email));
        userTable_->setItem(i, 4, new QTableWidgetItem(u.active ? "Yes" : "No"));
        userTable_->setItem(i, 5, new QTableWidgetItem(u.lastLogin.toString(Qt::ISODate)));
    }
}

void SecurityManagerPlugin::rebuildRoleTable() {
    if (!roleTable_)
        return;
    roleTable_->setRowCount(roles_.size());
    for (int i = 0; i < roles_.size(); ++i) {
        const auto& r = roles_[i];
        roleTable_->setItem(i, 0, new QTableWidgetItem(r.id));
        roleTable_->setItem(i, 1, new QTableWidgetItem(r.name));
        roleTable_->setItem(i, 2, new QTableWidgetItem(r.description));
        roleTable_->setItem(i, 3, new QTableWidgetItem(r.permissions.join(", ")));
    }
}

void SecurityManagerPlugin::rebuildPermissionTable() {
    if (!permissionTable_)
        return;
    permissionTable_->setRowCount(permissions_.size());
    for (int i = 0; i < permissions_.size(); ++i) {
        const auto& p = permissions_[i];
        permissionTable_->setItem(i, 0, new QTableWidgetItem(p.id));
        permissionTable_->setItem(i, 1, new QTableWidgetItem(p.resource));
        permissionTable_->setItem(i, 2, new QTableWidgetItem(p.action));
        permissionTable_->setItem(i, 3, new QTableWidgetItem(p.description));
    }
}

void SecurityManagerPlugin::rebuildAuditTable() {
    if (!auditTable_)
        return;
    auditTable_->setRowCount(filteredAudit_.size());
    for (int i = 0; i < filteredAudit_.size(); ++i) {
        const auto& e = auditLog_[filteredAudit_[i]];
        auditTable_->setItem(i, 0, new QTableWidgetItem(e.timestamp.toString(Qt::ISODate)));
        auditTable_->setItem(i, 1, new QTableWidgetItem(e.user));
        auditTable_->setItem(i, 2, new QTableWidgetItem(e.action));
        auditTable_->setItem(i, 3, new QTableWidgetItem(e.resource));
        auditTable_->setItem(i, 4, new QTableWidgetItem(e.details));
        auditTable_->setItem(i, 5, new QTableWidgetItem(e.severity));
    }
    if (statusLabel_)
        statusLabel_->setText(QString("Audit: %1 entries shown").arg(filteredAudit_.size()));
}
