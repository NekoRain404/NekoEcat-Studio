#include "ServiceTestFixture.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"
#include "services/ServiceContainer.h"

ServiceTestFixture::ServiceTestFixture(QObject* parent) : QObject(parent) {
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
}

ServiceTestFixture::~ServiceTestFixture() = default;

ServiceContainer* ServiceTestFixture::container() const {
    return container_;
}

void ServiceTestFixture::simulateConnection(bool connected) {
    container_->eventBus()->emitConnectionStateChanged(connected);
}

void ServiceTestFixture::simulateSlaveChange(int position, const QString& name, const QString& state) {
    SlaveInfo info;
    info.position = position;
    info.name = name;
    info.state = state;
    QVector<SlaveInfo> slaves{info};
    container_->eventBus()->emitSlaveChanged(slaves);
}

void ServiceTestFixture::simulateSdoValue(int position, const QString& index, const QString& subIndex,
                                          const QString& value) {
    container_->eventBus()->emitSdoValue(position, index, subIndex, value);
}
