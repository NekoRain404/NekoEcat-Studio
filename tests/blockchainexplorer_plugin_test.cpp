// BlockchainExplorerPluginTest — Tests for BlockchainExplorerPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Transactions CRUD
//   - Blocks CRUD
//   - Smart contracts CRUD
//   - Supply chain entries CRUD
//   - UI buttons do not synthesize blockchain records without backend evidence

#include <QFile>
#include <QPushButton>
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include "plugins/blockchainexplorer/BlockchainExplorerPlugin.h"

class BlockchainExplorerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, and default order
  void testPluginIdentity() {
    BlockchainExplorerPlugin plugin;
    QCOMPARE(plugin.id(), QString("blockchainexplorer"));
    QCOMPARE(plugin.displayName(), QString("Blockchain Explorer"));
    QCOMPARE(plugin.displayNameZh(), QString("区块链浏览器"));
    QCOMPARE(plugin.defaultOrder(), 375);
    QCOMPARE(plugin.visible(), true);
  }
  // Verify widget is created
  void testWidgetCreation() {
    BlockchainExplorerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }
  // Verify initial counts are zero
  void testInitialState() {
    BlockchainExplorerPlugin plugin;
    QCOMPARE(plugin.transactionCount(), 0);
    QCOMPARE(plugin.blockCount(), 0);
    QCOMPARE(plugin.smartContractCount(), 0);
    QCOMPARE(plugin.supplyChainEntryCount(), 0);
  }
  // Verify adding a transaction increments count
  void testAddTransaction() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::Transaction tx;
    tx.hash = "0xabc"; tx.from = "0x111"; tx.to = "0x222";
    tx.amount = 1.5; tx.timestamp = QDateTime::currentDateTime();
    tx.status = "confirmed";
    plugin.addTransaction(tx);
    QCOMPARE(plugin.transactionCount(), 1);
  }
  // Verify removing a transaction decrements count
  void testRemoveTransaction() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::Transaction tx;
    tx.hash = "0xabc"; tx.from = "0x111"; tx.to = "0x222";
    tx.amount = 1.5; tx.timestamp = QDateTime::currentDateTime();
    tx.status = "confirmed";
    plugin.addTransaction(tx);
    QCOMPARE(plugin.transactionCount(), 1);
    plugin.removeTransaction(0);
    QCOMPARE(plugin.transactionCount(), 0);
  }
  // Verify adding a block increments count
  void testAddBlock() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::BlockInfo b;
    b.number = 1; b.hash = "0x001"; b.previousHash = "0x000";
    b.transactionCount = 5; b.timestamp = QDateTime::currentDateTime();
    b.miner = "miner0";
    plugin.addBlock(b);
    QCOMPARE(plugin.blockCount(), 1);
  }
  // Verify removing a block decrements count
  void testRemoveBlock() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::BlockInfo b;
    b.number = 1; b.hash = "0x001"; b.previousHash = "0x000";
    b.transactionCount = 5; b.timestamp = QDateTime::currentDateTime();
    b.miner = "miner0";
    plugin.addBlock(b);
    QCOMPARE(plugin.blockCount(), 1);
    plugin.removeBlock(0);
    QCOMPARE(plugin.blockCount(), 0);
  }
  // Verify adding a smart contract increments count
  void testAddSmartContract() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::SmartContract c;
    c.address = "0xc0"; c.name = "Contract0"; c.owner = "owner0";
    c.deployedAt = QDateTime::currentDateTime();
    c.callCount = 0; c.active = true;
    plugin.addSmartContract(c);
    QCOMPARE(plugin.smartContractCount(), 1);
  }
  // Verify removing a smart contract decrements count
  void testRemoveSmartContract() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::SmartContract c;
    c.address = "0xc0"; c.name = "Contract0"; c.owner = "owner0";
    c.deployedAt = QDateTime::currentDateTime();
    c.callCount = 0; c.active = true;
    plugin.addSmartContract(c);
    QCOMPARE(plugin.smartContractCount(), 1);
    plugin.removeSmartContract(0);
    QCOMPARE(plugin.smartContractCount(), 0);
  }
  // Verify adding a supply chain entry increments count
  void testAddSupplyChainEntry() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::SupplyChainEntry e;
    e.itemId = "ITEM-0"; e.stage = "production";
    e.location = "Factory A"; e.timestamp = QDateTime::currentDateTime();
    e.handler = "handler0"; e.status = "in_transit";
    plugin.addSupplyChainEntry(e);
    QCOMPARE(plugin.supplyChainEntryCount(), 1);
  }
  // Verify removing a supply chain entry decrements count
  void testRemoveSupplyChainEntry() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::SupplyChainEntry e;
    e.itemId = "ITEM-0"; e.stage = "production";
    e.location = "Factory A"; e.timestamp = QDateTime::currentDateTime();
    e.handler = "handler0"; e.status = "in_transit";
    plugin.addSupplyChainEntry(e);
    QCOMPARE(plugin.supplyChainEntryCount(), 1);
    plugin.removeSupplyChainEntry(0);
    QCOMPARE(plugin.supplyChainEntryCount(), 0);
  }
  // Verify transaction filtering by status
  void testFilterTransactions() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::Transaction tx1;
    tx1.hash = "0xabc"; tx1.from = "0x111"; tx1.to = "0x222";
    tx1.amount = 1.5; tx1.timestamp = QDateTime::currentDateTime();
    tx1.status = "confirmed";
    plugin.addTransaction(tx1);
    BlockchainExplorerPlugin::Transaction tx2;
    tx2.hash = "0xdef"; tx2.from = "0x333"; tx2.to = "0x444";
    tx2.amount = 2.0; tx2.timestamp = QDateTime::currentDateTime();
    tx2.status = "pending";
    plugin.addTransaction(tx2);
    QCOMPARE(plugin.transactionCount(), 2);
    plugin.filterTransactions("pending");
    QCOMPARE(plugin.transactionCount(), 2);
  }
  // Verify tabs widget exists
  void testTabs() {
    BlockchainExplorerPlugin plugin;
    QVERIFY(plugin.tabs() != nullptr);
  }
  // Verify transaction table widget exists
  void testTransactionTable() {
    BlockchainExplorerPlugin plugin;
    QVERIFY(plugin.transactionTable() != nullptr);
  }
  // Verify block table widget exists
  void testBlockTable() {
    BlockchainExplorerPlugin plugin;
    QVERIFY(plugin.blockTable() != nullptr);
  }
  // Verify contract table widget exists
  void testContractTable() {
    BlockchainExplorerPlugin plugin;
    QVERIFY(plugin.contractTable() != nullptr);
  }
  // Verify supply chain table widget exists
  void testSupplyChainTable() {
    BlockchainExplorerPlugin plugin;
    QVERIFY(plugin.supplyChainTable() != nullptr);
  }
  // Verify export data produces non-empty JSON
  void testExportData() {
    BlockchainExplorerPlugin plugin;
    BlockchainExplorerPlugin::Transaction tx;
    tx.hash = "0xabc"; tx.from = "0x111"; tx.to = "0x222";
    tx.amount = 1.5; tx.timestamp = QDateTime::currentDateTime();
    tx.status = "confirmed";
    plugin.addTransaction(tx);
    QString json = plugin.exportData();
    QVERIFY(!json.isEmpty());
  }
  // Verify status label widget exists
  void testStatusLabel() {
    BlockchainExplorerPlugin plugin;
    QVERIFY(plugin.statusLabel() != nullptr);
  }
  // Verify transactionAdded signal is emitted
  void testTransactionAddedSignal() {
    BlockchainExplorerPlugin plugin;
    QSignalSpy spy(&plugin, &BlockchainExplorerPlugin::transactionAdded);
    BlockchainExplorerPlugin::Transaction tx;
    tx.hash = "0xabc"; tx.from = "0x111"; tx.to = "0x222";
    tx.amount = 1.5; tx.timestamp = QDateTime::currentDateTime();
    tx.status = "confirmed";
    plugin.addTransaction(tx);
    QCOMPARE(spy.count(), 1);
  }
  // Verify blockAdded signal is emitted
  void testBlockAddedSignal() {
    BlockchainExplorerPlugin plugin;
    QSignalSpy spy(&plugin, &BlockchainExplorerPlugin::blockAdded);
    BlockchainExplorerPlugin::BlockInfo b;
    b.number = 1; b.hash = "0x001"; b.previousHash = "0x000";
    b.transactionCount = 5; b.timestamp = QDateTime::currentDateTime();
    b.miner = "miner0";
    plugin.addBlock(b);
    QCOMPARE(spy.count(), 1);
  }

  void testUiButtonsDoNotSynthesizeBlockchainRecords() {
    BlockchainExplorerPlugin plugin;
    QWidget *widget = plugin.widget();
    const auto buttons = widget->findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
      if (button->text() == QStringLiteral("Add") ||
          button->text() == QStringLiteral("Execute")) {
        QTest::mouseClick(button, Qt::LeftButton);
      }
    }

    QCOMPARE(plugin.transactionCount(), 0);
    QCOMPARE(plugin.blockCount(), 0);
    QCOMPARE(plugin.smartContractCount(), 0);
    QCOMPARE(plugin.supplyChainEntryCount(), 0);
  }

  void testSourceDoesNotContainSyntheticBlockchainGenerators() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/blockchainexplorer/BlockchainExplorerPlugin.cpp"));
    QVERIFY(source.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("QRandomGenerator")),
             "Blockchain explorer must not synthesize random addresses");
    QVERIFY2(!text.contains(QStringLiteral("miner_")),
             "Blockchain explorer must not synthesize mined blocks");
    QVERIFY2(!text.contains(QStringLiteral("Contract_")),
             "Blockchain explorer must not synthesize smart contracts");
    QVERIFY2(!text.contains(QStringLiteral("ITEM-")),
             "Blockchain explorer must not synthesize supply-chain entries");
  }
};

QTEST_MAIN(BlockchainExplorerPluginTest)
#include "blockchainexplorer_plugin_test.moc"
