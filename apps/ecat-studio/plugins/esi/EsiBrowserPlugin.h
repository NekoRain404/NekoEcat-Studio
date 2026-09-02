#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSplitter;
class QTableWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QTabWidget;
class QTextEdit;
class EsiService;
class EsiParser;
class EsiDeviceMatcher;
struct EsiDeviceInfo;

class EsiBrowserPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit EsiBrowserPlugin(EsiService* esiService, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;
    void onConnectionChanged(bool connected) override;

    void importFile();
    void importDirectory();
    void refreshList();
    void exportSelected();
    void autoMatchDevices();

    EsiService* service() const { return service_; }
    EsiParser* parser() const { return parser_; }
    EsiDeviceMatcher* matcher() const { return matcher_; }

private:
    void buildUi();
    void buildEsiTree();
    void buildDetailPanel();
    void buildPdoPanel();
    void buildSyncManagerPanel();

    void updateDeviceList();
    void showDeviceDetail(int index);
    void showPdoMapping(const EsiDeviceInfo& dev);
    void showSyncManagerConfig(const EsiDeviceInfo& dev);
    void updateTreeFilter(const QString& filter);
    void onTreeSelectionChanged();

    EsiService* service_;
    EsiParser* parser_;
    EsiDeviceMatcher* matcher_;

    QWidget* containerWidget_ = nullptr;
    QSplitter* mainSplitter_ = nullptr;
    QSplitter* rightSplitter_ = nullptr;

    QTreeWidget* esiTree_ = nullptr;
    QLineEdit* filterEdit_ = nullptr;
    QTabWidget* detailTabs_ = nullptr;

    QTableWidget* detailTable_ = nullptr;
    QTableWidget* pdoTable_ = nullptr;
    QTableWidget* syncManagerTable_ = nullptr;
    QTextEdit* matchReport_ = nullptr;

    QLabel* summaryLabel_ = nullptr;
    QPushButton* importBtn_ = nullptr;
    QPushButton* importDirBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    QPushButton* refreshBtn_ = nullptr;
    QPushButton* matchBtn_ = nullptr;

    int currentDeviceIndex_ = -1;
    QVector<EsiDeviceInfo> currentDevices_;
};
