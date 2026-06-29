// SdoCachePluginTest — Tests for SDO Cache Plugin and PDO Mapping
//
// Test coverage:
//   - Cache value storage and retrieval
//   - Cache hit/miss tracking
//   - Cache invalidation (single slave and all)
//   - Dictionary caching
//   - LRU eviction on max size
//   - TTL expiration
//   - Plugin identity and signals
//   - PDO mapping validation
//   - PDO mapping export/import
#include <QtTest>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include "services/SdoCacheService.h"
#include "services/PdoMappingService.h"
#include "plugins/sdocache/SdoCachePlugin.h"

class SdoCachePluginTest : public QObject {
  Q_OBJECT
private slots:
  // Cache a value and retrieve it by slave/index/subindex
  void cacheValueAndRetrieve();
  // Track cache hits and misses accurately
  void cacheHitMissTracking();
  // Invalidate cache for a specific slave
  void invalidateCache();
  // Invalidate all cached entries across slaves
  void invalidateAll();
  // Cache and retrieve SDO dictionary entries
  void dictionaryCache();
  // LRU eviction when max entries per slave is exceeded
  void evictionOnMaxSize();
  // Entries expire after TTL elapses
  void ttlExpiration();
  // Plugin reports correct id, name, order, and visibility
  void pluginIdentity();
  // Cache update signal fires on cacheValue
  void pluginSignals();
  // Validate valid and invalid PDO mappings
  void pdoMappingValidation();
  // Export and import PDO mappings round-trip
  void pdoMappingExportImport();
  // Reject invalid PDO mapping import/export paths without clearing state
  void pdoMappingImportRejectsInvalidJson();
};

void SdoCachePluginTest::cacheValueAndRetrieve() {
  SdoCacheService svc;
  svc.cacheValue(0, "1000", "01", "AB");
  QCOMPARE(svc.getCachedValue(0, "1000", "01"), QString("AB"));
  QCOMPARE(svc.cacheSize(0), 1);
}

void SdoCachePluginTest::cacheHitMissTracking() {
  SdoCacheService svc;
  svc.cacheValue(0, "1000", "00", "1234");
  svc.getCachedValue(0, "1000", "00"); // hit
  svc.getCachedValue(0, "2000", "00"); // miss
  QCOMPARE(svc.hitCount(), 1LL);
  QCOMPARE(svc.missCount(), 1LL);
}

void SdoCachePluginTest::invalidateCache() {
  SdoCacheService svc;
  svc.cacheValue(1, "1000", "00", "AA");
  svc.cacheValue(1, "1000", "01", "BB");
  QCOMPARE(svc.cacheSize(1), 2);
  svc.invalidateCache(1);
  QCOMPARE(svc.cacheSize(1), 0);
}

void SdoCachePluginTest::invalidateAll() {
  SdoCacheService svc;
  svc.cacheValue(0, "1000", "00", "A");
  svc.cacheValue(1, "1000", "00", "B");
  svc.invalidateAll();
  QCOMPARE(svc.cachedSlaveCount(), 0);
  QCOMPARE(svc.hitCount(), 0LL);
}

void SdoCachePluginTest::dictionaryCache() {
  SdoCacheService svc;
  SdoDictionary dict;
  SdoDictionaryEntry e;
  e.index = "1000";
  e.subIndex = "00";
  e.name = "Device Type";
  dict.append(e);
  svc.cacheDictionary(0, dict);
  auto cached = svc.getCachedDictionary(0);
  QCOMPARE(cached.size(), 1);
  QCOMPARE(cached[0].name, QString("Device Type"));
}

void SdoCachePluginTest::evictionOnMaxSize() {
  SdoCacheService svc;
  SdoCacheConfig cfg;
  cfg.maxEntriesPerSlave = 3;
  cfg.ttlMs = 0;
  cfg.evictionPolicy = SdoCacheConfig::LRU;
  svc.setConfig(cfg);

  svc.cacheValue(0, "1000", "00", "A");
  svc.cacheValue(0, "1000", "01", "B");
  svc.cacheValue(0, "1000", "02", "C");
  svc.cacheValue(0, "1000", "03", "D");
  QVERIFY(svc.cacheSize(0) <= 3);
}

void SdoCachePluginTest::ttlExpiration() {
  SdoCacheService svc;
  SdoCacheConfig cfg;
  cfg.ttlMs = 1;
  svc.setConfig(cfg);
  svc.cacheValue(0, "1000", "00", "X");
  QTest::qSleep(10);
  // With 1ms TTL, the entry should be expired on next access
  // The service returns empty on miss
  svc.getCachedValue(0, "1000", "00");
  // After expiration, miss count increases
  QVERIFY(svc.missCount() >= 1);
}

void SdoCachePluginTest::pluginIdentity() {
  SdoCacheService svc;
  SdoCachePlugin plugin(&svc);
  QCOMPARE(plugin.id(), QString("sdocache"));
  QCOMPARE(plugin.displayName(), QString("SDO Cache"));
  QCOMPARE(plugin.defaultOrder(), 160);
  QVERIFY(plugin.visible());
  QVERIFY(plugin.widget() != nullptr);
}

void SdoCachePluginTest::pluginSignals() {
  SdoCacheService svc;
  QSignalSpy spy(&svc, &SdoCacheService::cacheUpdated);
  svc.cacheValue(0, "1000", "00", "V");
  QCOMPARE(spy.count(), 1);
  QCOMPARE(spy[0][0].toInt(), 0);
}

void SdoCachePluginTest::pdoMappingValidation() {
  PdoMappingService svc;
  PdoMapping m;
  m.index = "0x1600";
  m.subIndex = "0x01";
  m.bitSize = 16;
  m.direction = PdoDirection::Output;
  m.slavePosition = 0;
  auto r = svc.validateMapping(m);
  QVERIFY(r.valid);

  PdoMapping bad;
  bad.bitSize = 0;
  auto r2 = svc.validateMapping(bad);
  QVERIFY(!r2.valid);
}

void SdoCachePluginTest::pdoMappingExportImport() {
  PdoMappingService svc;
  PdoMapping m;
  m.index = "0x1A00";
  m.subIndex = "0x01";
  m.name = "StatusWord";
  m.dataType = "UINT16";
  m.bitSize = 16;
  m.direction = PdoDirection::Input;
  m.slavePosition = 0;
  m.enabled = true;
  QVERIFY(svc.configureMapping(0, m));

  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString path = dir.filePath(QStringLiteral("pdo_test_export.json"));
  QVERIFY(svc.exportMapping(0, path));

  PdoMappingService imported;
  QVERIFY(imported.importMapping(0, path));
  auto mappings = imported.currentMappings(0);
  QCOMPARE(mappings.size(), 1);
  QCOMPARE(mappings.first().index, QStringLiteral("0x1A00"));
  QCOMPARE(mappings.first().subIndex, QStringLiteral("0x01"));
  QCOMPARE(mappings.first().name, QStringLiteral("StatusWord"));
  QCOMPARE(mappings.first().dataType, QStringLiteral("UINT16"));
  QCOMPARE(mappings.first().bitSize, 16);
  QCOMPARE(mappings.first().direction, PdoDirection::Input);
  QVERIFY(mappings.first().enabled);
}

void SdoCachePluginTest::pdoMappingImportRejectsInvalidJson() {
  PdoMappingService svc;
  PdoMapping m;
  m.index = "0x1A00";
  m.subIndex = "0x01";
  m.name = "StatusWord";
  m.dataType = "UINT16";
  m.bitSize = 16;
  m.direction = PdoDirection::Input;
  m.slavePosition = 0;
  m.enabled = true;
  QVERIFY(svc.configureMapping(0, m));

  QVERIFY(!svc.exportMapping(0, QString()));

  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  QVERIFY(!svc.exportMapping(0, dir.path()));

  const QString invalidPath = dir.filePath(QStringLiteral("invalid.json"));
  QFile invalidFile(invalidPath);
  QVERIFY(invalidFile.open(QIODevice::WriteOnly));
  QCOMPARE(invalidFile.write(QByteArrayLiteral("[]")), 2);
  invalidFile.close();

  QVERIFY(!svc.importMapping(0, QString()));
  QVERIFY(!svc.importMapping(0, invalidPath));
  auto mappings = svc.currentMappings(0);
  QCOMPARE(mappings.size(), 1);
  QCOMPARE(mappings.first().index, QStringLiteral("0x1A00"));
}

static int argc = 1;
static char arg0[] = "test";
static char *argv[] = {arg0, nullptr};

QTEST_MAIN(SdoCachePluginTest)
#include "sdo_cache_plugin_test.moc"
