#pragma once

// WorkspacePlugin — common interface for all workspace tabs.
// Each workspace (Overview, Object Dictionary, Watch, etc.) implements
// this interface and registers with PluginRegistry.

#include <QObject>
#include <QString>
#include <QWidget>
#include <QIcon>

struct AppSettings;

class WorkspacePlugin : public QObject {
    Q_OBJECT
public:
    virtual ~WorkspacePlugin() = default;

    // Identity
    virtual QString id() const = 0;
    virtual QString displayName() const = 0;
    virtual QString displayNameZh() const = 0;
    virtual QIcon icon() const { return QIcon(); }

    // UI
    virtual QWidget *widget() = 0;
    virtual int defaultOrder() const = 0;
    virtual bool visible() const = 0;

    // Lifecycle
    virtual void activate() {}
    virtual void deactivate() {}
    virtual void onSettingsChanged(const AppSettings &) {}
    virtual void onConnectionChanged(bool connected) {}

signals:
    void requestNavigate(const QString &pluginId);
    void updateDiagnostics(const QString &level, const QString &source, const QString &msg);
};
