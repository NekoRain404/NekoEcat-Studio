// AdapterHandlerTest — Tests for AdapterHandler
//
// Test coverage:
//   - Network adapter discovery and listing
//   - Adapter field validation
//   - Set adapter rejection (empty/invalid)
//   - Loopback exclusion from list
//   - Success/failure envelope structure (ok / result / error)

#include <QTest>
#include <QJsonObject>
#include <QJsonArray>

#include "../../../apps/ecatd/handlers/AdapterHandler.h"

class AdapterHandlerTest : public QObject {
  Q_OBJECT

private slots:
  // Verify handleList returns the expected JSON shape even with no real adapters.
  void testHandleListStructure() {
    AdapterHandler handler;
    QJsonObject resp = handler.handleList("1", {});

    // Response must be a success envelope containing 'adapters' and 'current'.
    QVERIFY(resp.value("ok").toBool() == true);
    QJsonObject result = resp.value("result").toObject();
    QVERIFY(result.contains("adapters"));
    QVERIFY(result.contains("current"));
    QVERIFY(result["adapters"].isArray());
    // 'current' is a string (may be empty if /etc/ethercat.conf is absent).
    QVERIFY(result["current"].isString());
  }

  // Verify that every adapter in the list has the required fields.
  void testAdapterFieldsPresent() {
    AdapterHandler handler;
    QJsonObject resp = handler.handleList("2", {});

    QJsonArray adapters = resp.value("result").toObject()["adapters"].toArray();
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

  // Verify handleSet rejects an empty adapter parameter with a failure envelope.
  void testHandleSetEmptyAdapter() {
    AdapterHandler handler;
    QJsonObject resp = handler.handleSet("3", {});

    QVERIFY(resp.value("ok").toBool() == false);
    QVERIFY(resp.contains("error"));
    QJsonObject err = resp.value("error").toObject();
    QVERIFY(err["message"].toString().contains("Missing"));
    QCOMPARE(err["code"].toInt(), -1);
  }

  // Verify handleSet rejects a non-existent adapter name.
  void testHandleSetInvalidAdapter() {
    AdapterHandler handler;
    QJsonObject params;
    params["adapter"] = "nonexistent_iface_99999";
    QJsonObject resp = handler.handleSet("4", params);

    QVERIFY(resp.value("ok").toBool() == false);
    QJsonObject err = resp.value("error").toObject();
    QVERIFY(err["message"].toString().contains("not found"));
    QCOMPARE(err["code"].toInt(), -1);
  }

  // Verify that enumerateAdapters does not include loopback.
  void testNoLoopbackInList() {
    AdapterHandler handler;
    QJsonObject resp = handler.handleList("5", {});

    QJsonArray adapters = resp.value("result").toObject()["adapters"].toArray();
    for (const auto &entry : adapters) {
      QJsonObject obj = entry.toObject();
      QVERIFY(obj["name"].toString() != "lo");
    }
  }
};

QTEST_MAIN(AdapterHandlerTest)
#include "adapter_handler_test.moc"
