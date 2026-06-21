// QuantumSecurityPluginTest — Tests for QuantumSecurityPlugin (signal-based)
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Widget creation
//   - Initial state (keys, randoms, encryptions, signatures)
//   - Key add/remove with signals
//   - Quantum random number add/remove
//   - Encryption add/remove with signals
//   - Signature add/remove/verify with signals
//   - Tab widget and table creation
//   - Export report and status label

// QuantumSecurityPluginTest — Tests for QuantumSecurityPlugin (second variant)
//
// Test coverage:
//   - Plugin identity and widget creation
//   - Initial state (key, random, encryption, signature counts)
//   - Key, random, encryption, signature add/remove operations
//   - Signature verification with signal
//   - Tab, table, and status label widget access
//   - JSON report export
//   - Signal emissions for keyGenerated and encryptionCompleted
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include "plugins/quantumsecurity/QuantumSecurityPlugin.h"

class QuantumSecurityPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, displayName, displayNameZh, defaultOrder, visible
  void testPluginIdentity() {
    QuantumSecurityPlugin plugin;
    QCOMPARE(plugin.id(), QString("quantumsecurity"));
    QCOMPARE(plugin.displayName(), QString("Quantum Security"));
    QCOMPARE(plugin.displayNameZh(), QString("量子安全"));
    QCOMPARE(plugin.defaultOrder(), 380);
    QCOMPARE(plugin.visible(), true);
  }
  // Check widget is created
  // Verify widget is created and non-null
  void testWidgetCreation() {
    QuantumSecurityPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }
  // Verify all initial counts are zero
  // Verify all counts start at zero
  void testInitialState() {
    QuantumSecurityPlugin plugin;
    QCOMPARE(plugin.keyCount(), 0);
    QCOMPARE(plugin.randomCount(), 0);
    QCOMPARE(plugin.encryptionCount(), 0);
    QCOMPARE(plugin.signatureCount(), 0);
  }
  // Test adding a quantum key
  // Test adding a quantum key increments keyCount
  void testAddKey() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumKey k;
    k.id = "qkey_0"; k.algorithm = "BB84"; k.keySize = 256;
    k.createdAt = QDateTime::currentDateTime();
    k.expiresAt = QDateTime::currentDateTime().addDays(30);
    k.active = true;
    plugin.addKey(k);
    QCOMPARE(plugin.keyCount(), 1);
  }
  // Test removing a quantum key
  // Test removing a quantum key decrements keyCount
  void testRemoveKey() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumKey k;
    k.id = "qkey_0"; k.algorithm = "BB84"; k.keySize = 256;
    k.createdAt = QDateTime::currentDateTime();
    k.expiresAt = QDateTime::currentDateTime().addDays(30);
    k.active = true;
    plugin.addKey(k);
    QCOMPARE(plugin.keyCount(), 1);
    plugin.removeKey(0);
    QCOMPARE(plugin.keyCount(), 0);
  }
  // Test adding a quantum random entry
  // Test adding a quantum random entry increments randomCount
  void testAddRandom() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumRandom r;
    r.id = "qrng_0"; r.bitLength = 256;
    r.entropy = "quantum_vacuum"; r.generatedAt = QDateTime::currentDateTime();
    r.source = "QRNG";
    plugin.addRandom(r);
    QCOMPARE(plugin.randomCount(), 1);
  }
  // Test removing a quantum random entry
  // Test removing a quantum random entry decrements randomCount
  void testRemoveRandom() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumRandom r;
    r.id = "qrng_0"; r.bitLength = 256;
    r.entropy = "quantum_vacuum"; r.generatedAt = QDateTime::currentDateTime();
    r.source = "QRNG";
    plugin.addRandom(r);
    QCOMPARE(plugin.randomCount(), 1);
    plugin.removeRandom(0);
    QCOMPARE(plugin.randomCount(), 0);
  }
  // Test adding an encryption entry
  // Test adding an encryption entry increments encryptionCount
  void testAddEncryption() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumEncryption e;
    e.id = "qenc_0"; e.algorithm = "AES-256-QKD";
    e.inputHash = "0x111"; e.outputHash = "0x222";
    e.timestamp = QDateTime::currentDateTime(); e.success = true;
    plugin.addEncryption(e);
    QCOMPARE(plugin.encryptionCount(), 1);
  }
  // Test removing an encryption entry
  // Test removing an encryption entry decrements encryptionCount
  void testRemoveEncryption() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumEncryption e;
    e.id = "qenc_0"; e.algorithm = "AES-256-QKD";
    e.inputHash = "0x111"; e.outputHash = "0x222";
    e.timestamp = QDateTime::currentDateTime(); e.success = true;
    plugin.addEncryption(e);
    QCOMPARE(plugin.encryptionCount(), 1);
    plugin.removeEncryption(0);
    QCOMPARE(plugin.encryptionCount(), 0);
  }
  // Test adding a signature entry
  // Test adding a signature entry increments signatureCount
  void testAddSignature() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumSignature s;
    s.id = "qsig_0"; s.signer = "signer0";
    s.messageHash = "0xaaa"; s.signature = "0xbbb";
    s.timestamp = QDateTime::currentDateTime(); s.verified = false;
    plugin.addSignature(s);
    QCOMPARE(plugin.signatureCount(), 1);
  }
  // Test removing a signature entry
  // Test removing a signature entry decrements signatureCount
  void testRemoveSignature() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumSignature s;
    s.id = "qsig_0"; s.signer = "signer0";
    s.messageHash = "0xaaa"; s.signature = "0xbbb";
    s.timestamp = QDateTime::currentDateTime(); s.verified = false;
    plugin.addSignature(s);
    QCOMPARE(plugin.signatureCount(), 1);
    plugin.removeSignature(0);
    QCOMPARE(plugin.signatureCount(), 0);
  }
  // Test signature verification with signal
  // Verify signature verification emits signatureVerified signal
  void testVerifySignature() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumSignature s;
    s.id = "qsig_0"; s.signer = "signer0";
    s.messageHash = "0xaaa"; s.signature = "0xbbb";
    s.timestamp = QDateTime::currentDateTime(); s.verified = false;
    plugin.addSignature(s);
    QSignalSpy spy(&plugin, &QuantumSecurityPlugin::signatureVerified);
    plugin.verifySignature(0);
    QCOMPARE(spy.count(), 1);
  }
  // Check tab widget exists
  // Verify tabs widget is non-null
  void testTabs() {
    QuantumSecurityPlugin plugin;
    QVERIFY(plugin.tabs() != nullptr);
  }
  // Check key table exists
  // Verify key table widget is non-null
  void testKeyTable() {
    QuantumSecurityPlugin plugin;
    QVERIFY(plugin.keyTable() != nullptr);
  }
  // Check random table exists
  // Verify random table widget is non-null
  void testRandomTable() {
    QuantumSecurityPlugin plugin;
    QVERIFY(plugin.randomTable() != nullptr);
  }
  // Check encryption table exists
  // Verify encryption table widget is non-null
  void testEncryptionTable() {
    QuantumSecurityPlugin plugin;
    QVERIFY(plugin.encryptionTable() != nullptr);
  }
  // Check signature table exists
  // Verify signature table widget is non-null
  void testSignatureTable() {
    QuantumSecurityPlugin plugin;
    QVERIFY(plugin.signatureTable() != nullptr);
  }
  // Test report export content
  // Test JSON report export returns non-empty string
  void testExportReport() {
    QuantumSecurityPlugin plugin;
    QuantumSecurityPlugin::QuantumKey k;
    k.id = "qkey_0"; k.algorithm = "BB84"; k.keySize = 256;
    k.createdAt = QDateTime::currentDateTime();
    k.expiresAt = QDateTime::currentDateTime().addDays(30);
    k.active = true;
    plugin.addKey(k);
    QString json = plugin.exportReport();
    QVERIFY(!json.isEmpty());
  }
  // Check status label exists
  // Verify status label widget is non-null
  void testStatusLabel() {
    QuantumSecurityPlugin plugin;
    QVERIFY(plugin.statusLabel() != nullptr);
  }
  // Verify keyGenerated signal on key add
  // Verify keyGenerated signal is emitted on addKey
  void testKeyGeneratedSignal() {
    QuantumSecurityPlugin plugin;
    QSignalSpy spy(&plugin, &QuantumSecurityPlugin::keyGenerated);
    QuantumSecurityPlugin::QuantumKey k;
    k.id = "qkey_0"; k.algorithm = "BB84"; k.keySize = 256;
    k.createdAt = QDateTime::currentDateTime();
    k.expiresAt = QDateTime::currentDateTime().addDays(30);
    k.active = true;
    plugin.addKey(k);
    QCOMPARE(spy.count(), 1);
  }
  // Verify encryptionCompleted signal on encryption add
  // Verify encryptionCompleted signal is emitted on addEncryption
  void testEncryptionCompletedSignal() {
    QuantumSecurityPlugin plugin;
    QSignalSpy spy(&plugin, &QuantumSecurityPlugin::encryptionCompleted);
    QuantumSecurityPlugin::QuantumEncryption e;
    e.id = "qenc_0"; e.algorithm = "AES-256-QKD";
    e.inputHash = "0x111"; e.outputHash = "0x222";
    e.timestamp = QDateTime::currentDateTime(); e.success = true;
    plugin.addEncryption(e);
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(QuantumSecurityPluginTest)
#include "quantumsecurity_plugin_test.moc"
