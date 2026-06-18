#pragma once

// WatchService — manages the Watch list, periodic polling, and drift detection.

#include <QObject>
#include <QString>
#include <QVector>

class EcatClient;

struct WatchEntry {
  int position = -1;
  QString index;
  QString subIndex;
  QString type;
  QString value;
  QString previousValue;
  bool changed = false;
};

class WatchService : public QObject {
  Q_OBJECT
public:
  explicit WatchService(EcatClient *client, QObject *parent = nullptr);

  void addEntry(int position, const QString &index, const QString &subIndex, const QString &type = QString());
  void removeEntry(int position, const QString &index, const QString &subIndex);
  void refreshAll();
  int entryCount() const;
  const WatchEntry &entryAt(int i) const;

signals:
  void entryUpdated(int row, const WatchEntry &entry);
  void refreshComplete(int requested, int succeeded);

private:
  EcatClient *client_;
  QVector<WatchEntry> entries_;
};
