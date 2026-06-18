#include "SdoService.h"
#include "EcatClient.h"

SdoService::SdoService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
  connect(client_, &EcatClient::sdoValue, this,
          [this](int pos, const QString &idx, const QString &sub, const QString &val) {
            emit sdoValueReceived(pos, idx, sub, val);
          });
  connect(client_, &EcatClient::errorMessage, this,
          [this](const QString &msg) { emit error(msg); });
}

void SdoService::upload(int position, const QString &index, const QString &subIndex) {
  client_->upload(position, index, subIndex);
}

void SdoService::download(int position, const QString &index, const QString &subIndex,
                          const QString &value, const QString &type) {
  client_->download(position, index, subIndex, value, type);
}
