#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QMap>
#include <QString>
#include <QVector>

class QLabel;
class QListWidget;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QWidget;

class DashboardDesignerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit DashboardDesignerPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    struct DashboardWidget {
        QString id;
        QString type;
        QString label;
        int x = 0;
        int y = 0;
        int width = 100;
        int height = 100;
        QMap<QString, QString> properties;
    };

    void addWidget(const QString& type);
    void removeWidget(int index);
    void selectWidget(int index);
    void updateWidgetProperty(int index, const QString& key, const QString& value);
    int widgetCount() const;
    int selectedWidget() const;

    QString exportConfiguration() const;
    void importConfiguration(const QString& json);

    void setPreviewMode(bool preview);
    bool isPreviewMode() const;

    QListWidget* widgetPalette() const;
    QTableWidget* propertyEditor() const;
    QTabWidget* modeTabs() const;
    QLabel* statusLabel() const;

signals:
    void widgetSelected(int index);
    void widgetModified(int index);

private:
    void buildUi();
    void rebuildPropertyEditor();
    void rebuildCanvas();

    QWidget* containerWidget_ = nullptr;
    QListWidget* widgetPalette_ = nullptr;
    QTableWidget* propertyEditor_ = nullptr;
    QTabWidget* modeTabs_ = nullptr;
    QWidget* canvas_ = nullptr;
    QLabel* statusLabel_ = nullptr;

    QVector<DashboardWidget> widgets_;
    int selectedIndex_ = -1;
    bool previewMode_ = false;
    int nextId_ = 1;
};
