#pragma once

// AlEventService — polls the ecatd daemon for AL event log entries and emits
// parsed results.  Also supports clearing the remote event log.  Intended for
// use by AlEventPlugin to populate its workspace table.

#include <QObject>
#include <QJsonObject>

class QTimer;
class EcatClient;

class AlEventService : public QObject {
  Q_OBJECT
public:
  explicit AlEventService(EcatClient *client, QObject *parent = nullptr);

  // Start periodic polling.  intervalMs defaults to 2000 ms.
  void startPolling(int intervalMs = 2000);
  void stopPolling();

  // Issue a single log request to the daemon.
  void requestUpdate();

  // Clear all AL events on the daemon side.
  void clearEvents();

signals:
  // Emitted after each successful daemon log response.
  void alEventUpdate(const QJsonObject &data);
  void error(const QString &message);

private:
  EcatClient *client_;
  QTimer *pollTimer_;
};
