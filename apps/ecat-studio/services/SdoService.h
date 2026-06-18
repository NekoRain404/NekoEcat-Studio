#pragma once

// SdoService — manages SDO read/write operations, dictionary caching, and evidence tracking.

#include <QObject>
#include <QString>

class EcatClient;

class SdoService : public QObject {
  Q_OBJECT
public:
  explicit SdoService(EcatClient *client, QObject *parent = nullptr);

  void upload(int position, const QString &index, const QString &subIndex);
  void download(int position, const QString &index, const QString &subIndex,
                const QString &value, const QString &type);

signals:
  void sdoValueReceived(int position, const QString &index, const QString &subIndex, const QString &value);
  void error(const QString &message);

private:
  EcatClient *client_;
};
