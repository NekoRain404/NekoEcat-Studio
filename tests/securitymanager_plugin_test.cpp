// SecurityManagerPluginTest — Tests for Security Manager Plugin (securitymanager)
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Initial state (users, roles, permissions, audits)
//   - User/role/permission/audit table structure
//   - Add/remove/update operations for users, roles, permissions
//   - Audit entry management and filtering
//   - Status label and report export
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QLabel>
#include "plugins/securitymanager/SecurityManagerPlugin.h"

class SecurityManagerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin ID, display name, order, and visibility
  void testPluginIdentity() {
    SecurityManagerPlugin plugin;

    QCOMPARE(plugin.id(), QString("securitymanager"));
    QCOMPARE(plugin.displayName(), QString("Security Manager"));
    QCOMPARE(plugin.displayNameZh(), QString("安全管理器"));
    QCOMPARE(plugin.defaultOrder(), 280);
    QCOMPARE(plugin.visible(), true);
  }

  // Widget should be created successfully
  void testWidgetCreation() {
    SecurityManagerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Default counts for users, roles, permissions, and audits
  void testInitialState() {
    SecurityManagerPlugin plugin;

    QCOMPARE(plugin.userCount(), 3);
    QCOMPARE(plugin.roleCount(), 3);
    QCOMPARE(plugin.permissionCount(), 7);
    QCOMPARE(plugin.auditCount(), 2);
  }

  // User table has correct dimensions
  void testUserTable() {
    SecurityManagerPlugin plugin;

    QTableWidget *table = plugin.userTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 6);
  }

  // Role table has correct dimensions
  void testRoleTable() {
    SecurityManagerPlugin plugin;

    QTableWidget *table = plugin.roleTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 4);
  }

  // Permission table has correct dimensions
  void testPermissionTable() {
    SecurityManagerPlugin plugin;

    QTableWidget *table = plugin.permissionTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 7);
    QCOMPARE(table->columnCount(), 4);
  }

  // Audit table has correct dimensions
  void testAuditTable() {
    SecurityManagerPlugin plugin;

    QTableWidget *table = plugin.auditTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 2);
    QCOMPARE(table->columnCount(), 6);
  }

  // Add a new user and verify signal emission
  void testAddUser() {
    SecurityManagerPlugin plugin;
    QSignalSpy spy(&plugin, &SecurityManagerPlugin::userAdded);
    int initial = plugin.userCount();

    SecurityManagerPlugin::User u;
    u.id = "u_new";
    u.username = "testuser";
    u.role = "Viewer";
    u.email = "test@test.com";
    u.active = true;

    plugin.addUser(u);
    QCOMPARE(plugin.userCount(), initial + 1);
    QCOMPARE(spy.count(), 1);
  }

  // Remove a user and verify signal emission
  void testRemoveUser() {
    SecurityManagerPlugin plugin;
    QSignalSpy spy(&plugin, &SecurityManagerPlugin::userRemoved);
    int initial = plugin.userCount();

    plugin.removeUser(0);
    QCOMPARE(plugin.userCount(), initial - 1);
    QCOMPARE(spy.count(), 1);
  }

  // Update user details at a given index
  void testUpdateUser() {
    SecurityManagerPlugin plugin;

    SecurityManagerPlugin::User u;
    u.id = "u1";
    u.username = "updated_admin";
    u.role = "Administrator";
    u.email = "new@email.com";
    u.active = false;

    plugin.updateUser(0, u);
    QCOMPARE(plugin.userTable()->item(0, 1)->text(), QString("updated_admin"));
  }

  // Add a new role with permissions
  void testAddRole() {
    SecurityManagerPlugin plugin;
    int initial = plugin.roleCount();

    SecurityManagerPlugin::Role r;
    r.id = "r_new";
    r.name = "Custom Role";
    r.description = "Custom";
    r.permissions = {"read", "write"};

    plugin.addRole(r);
    QCOMPARE(plugin.roleCount(), initial + 1);
  }

  // Remove a role by index
  void testRemoveRole() {
    SecurityManagerPlugin plugin;
    int initial = plugin.roleCount();

    plugin.removeRole(0);
    QCOMPARE(plugin.roleCount(), initial - 1);
  }

  // Add a new permission entry
  void testAddPermission() {
    SecurityManagerPlugin plugin;
    int initial = plugin.permissionCount();

    SecurityManagerPlugin::Permission p;
    p.id = "p_new";
    p.resource = "test";
    p.action = "read";
    p.description = "Test permission";

    plugin.addPermission(p);
    QCOMPARE(plugin.permissionCount(), initial + 1);
  }

  // Remove a permission by index
  void testRemovePermission() {
    SecurityManagerPlugin plugin;
    int initial = plugin.permissionCount();

    plugin.removePermission(0);
    QCOMPARE(plugin.permissionCount(), initial - 1);
  }

  // Add audit entry and verify signal emission
  void testAddAuditEntry() {
    SecurityManagerPlugin plugin;
    QSignalSpy spy(&plugin, &SecurityManagerPlugin::auditEntryAdded);
    int initial = plugin.auditCount();

    SecurityManagerPlugin::AuditEntry e;
    e.timestamp = QDateTime::currentDateTime();
    e.user = "test";
    e.action = "test_action";
    e.resource = "test_resource";
    e.details = "test details";
    e.severity = "info";

    plugin.addAuditEntry(e);
    QCOMPARE(plugin.auditCount(), initial + 1);
    QCOMPARE(spy.count(), 1);
  }

  // Filter audit log by user and severity
  void testFilterAudit() {
    SecurityManagerPlugin plugin;

    plugin.filterAudit("admin", "All");
    QVERIFY(plugin.auditTable()->rowCount() >= 1);
  }

  // Status label is created and accessible
  void testStatusLabel() {
    SecurityManagerPlugin plugin;

    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Export security report to file and verify creation
  void testExportReport() {
    SecurityManagerPlugin plugin;

    QString path = QDir::temp().absoluteFilePath("security_report_test.txt");
    plugin.exportSecurityReport(path);
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(SecurityManagerPluginTest)
#include "securitymanager_plugin_test.moc"
