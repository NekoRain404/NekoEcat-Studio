#pragma once

// DcSyncService — polls the daemon for DC sync diagnostics and emits
// parsed results.  Intended for use by DcSyncPlugin to populate its
// workspace table with per-slave timing information.

#include <QObject>
#include <QJsonObject>

class QTimer;
class EcatClient;

class DcSyncService : public QObject {
  Q_OBJECT
public:
  explicit DcSyncService(EcatClient *client, QObject *parent = nullptr);

  // Start periodic polling.  intervalMs defaults to 2000 ms.
  void startPolling(int intervalMs = 2000);
  void stopPolling();

  // Issue a single request to the daemon.
  void requestUpdate();

signals:
  // Emitted after each successful daemon response.
  void dcSyncUpdate(const QJsonObject &data);
  void error(const QString &message);

private:
  EcatClient *client_;
  QTimer *pollTimer_;
};
