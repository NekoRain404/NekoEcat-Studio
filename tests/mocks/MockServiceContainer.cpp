#include "MockServiceContainer.h"
#include "MockEcatClient.h"
#include "MockEventBus.h"

MockServiceContainer::MockServiceContainer(QObject* parent)
    : ServiceContainer(new MockEcatClient(nullptr), new MockEventBus(nullptr), parent) {
    mockClient_ = static_cast<MockEcatClient*>(client());
    mockEventBus_ = static_cast<MockEventBus*>(eventBus());
}

MockServiceContainer::~MockServiceContainer() = default;

MockEcatClient* MockServiceContainer::mockClient() const {
    return mockClient_;
}

MockEventBus* MockServiceContainer::mockEventBus() const {
    return mockEventBus_;
}
