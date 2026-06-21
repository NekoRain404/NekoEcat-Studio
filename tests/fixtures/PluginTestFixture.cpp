#include "PluginTestFixture.h"
#include "plugins/WorkspacePlugin.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"

PluginTestFixture::PluginTestFixture(QObject *parent)
    : QObject(parent)
{
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
    registry_ = new PluginRegistry();
}

PluginTestFixture::~PluginTestFixture()
{
    delete registry_;
}

ServiceContainer *PluginTestFixture::container() const
{
    return container_;
}

PluginRegistry *PluginTestFixture::registry() const
{
    return registry_;
}

void PluginTestFixture::registerPlugin(WorkspacePlugin *plugin)
{
    registry_->registerPlugin(plugin);
}

void PluginTestFixture::activatePlugin(const QString &id)
{
    auto *p = findPlugin(id);
    if (p)
        p->activate();
}

void PluginTestFixture::deactivatePlugin(const QString &id)
{
    auto *p = findPlugin(id);
    if (p)
        p->deactivate();
}

WorkspacePlugin *PluginTestFixture::findPlugin(const QString &id) const
{
    return registry_->findById(id);
}

int PluginTestFixture::pluginCount() const
{
    return registry_->count();
}
