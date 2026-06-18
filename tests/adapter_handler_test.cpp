// Tests for AdapterHandler — network adapter discovery and configuration.

#include <QTest>
#include <QJsonObject>
#include <QJsonArray>

#include "../apps/ecatd/handlers/AdapterHandler.h"

class AdapterHandlerTest : public QObject {
  Q_OBJECT

private slots:
  // Verify handleList returns the expected JSON shape even with no real adapters.
  void testHandleListStructure() {
    AdapterHandler handler;
    QJsonObject result = handler.handleList("1", {});

    // Response must contain 'adapters' array and 'current' string.
    QVERIFY(result.contains("adapters"));
    QVERIFY(result.contains("current"));
    QVERIFY(result["adapters"].isArray());
    // 'current' is a string (may be empty if /etc/ethercat.conf is absent).
    QVERIFY(result["current"].isString());
  }

  // Verify that every adapter in the list has the required fields.
  void testAdapterFieldsPresent() {
    AdapterHandler handler;
    QJsonObject result = handler.handleList("2", {});

    QJsonArray adapters = result["adapters"].toArray();
    for (const auto &entry : adapters) {
      QJsonObject obj = entry.toObject();
      QVERIFY(obj.contains("name"));
      QVERIFY(obj.contains("mac"));
      QVERIFY(obj.contains("driver"));
      QVERIFY(obj.contains("pciSlot"));
      QVERIFY(obj.contains("linkUp"));
      QVERIFY(obj.contains("isEthercat"));
      // name should be non-empty on any real adapter.
      QVERIFY(!obj["name"].toString().isEmpty());
    }
  }

  // Verify handleSet rejects an empty adapter parameter.
  void testHandleSetEmptyAdapter() {
    AdapterHandler handler;
    QJsonObject result = handler.handleSet("3", {});

    QVERIFY(result.contains("error"));
    QVERIFY(result["error"].toString().contains("Missing"));
    QCOMPARE(result["code"].toInt(), -1);
  }

  // Verify handleSet rejects a non-existent adapter name.
  void testHandleSetInvalidAdapter() {
    AdapterHandler handler;
    QJsonObject params;
    params["adapter"] = "nonexistent_iface_99999";
    QJsonObject result = handler.handleSet("4", params);

    QVERIFY(result.contains("error"));
    QVERIFY(result["error"].toString().contains("not found"));
    QCOMPARE(result["code"].toInt(), -1);
  }

  // Verify that enumerateAdapters does not include loopback.
  void testNoLoopbackInList() {
    AdapterHandler handler;
    QJsonObject result = handler.handleList("5", {});

    QJsonArray adapters = result["adapters"].toArray();
    for (const auto &entry : adapters) {
      QJsonObject obj = entry.toObject();
      QVERIFY(obj["name"].toString() != "lo");
    }
  }
};

QTEST_MAIN(AdapterHandlerTest)
#include "adapter_handler_test.moc"
