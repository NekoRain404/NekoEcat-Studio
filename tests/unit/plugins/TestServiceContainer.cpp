// Minimal ServiceContainer stub for plugin unit tests.
// Replaces the full ServiceContainer.cpp (which instantiates 85+ services)
// with a version that just stores the passed-in pointers. The plugin tests
// in this directory only exercise plugin identity/UI methods and don't
// call into the sub-services, so this stub is sufficient for compilation
// and linking.

#include "services/ServiceContainer.h"

ServiceContainer::ServiceContainer(EcatClient *client, EventBus *eventBus, QObject *parent)
    : QObject(parent),
      client_(client),
      eventBus_(eventBus)
{
    // All sub-services intentionally omitted — they are not exercised by
    // identity/UI plugin tests and would require linking ~70 service .cpp files.
    initialized_ = true;
}
