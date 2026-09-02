#pragma once

// ReportPlugin — report generation workspace for EtherCAT diagnostics and configuration.

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class ReportPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit ReportPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    struct ReportTemplate {
        QString id;
        QString name;
        QString description;
        QString format;
    };

    struct DataSource {
        QString id;
        QString name;
        QString type;
        bool enabled;
    };

    struct ReportRecord {
        QString id;
        QString templateName;
        QString format;
        QDateTime generatedAt;
        QString filePath;
    };

    void addReportTemplate(const ReportTemplate& tmpl);
    int reportTemplateCount() const;

    void addDataSource(const DataSource& source);
    void toggleDataSource(int index, bool enabled);
    int dataSourceCount() const;

    void selectTemplate(int index);
    void selectFormat(const QString& format);

    void generateReport();
    int reportCount() const;

    QTableWidget* templateTable() const;
    QTableWidget* dataSourceTable() const;
    QTableWidget* historyTable() const;
    QTextEdit* previewView() const;
    QLabel* statusLabel() const;

signals:
    void reportGenerated(const QString& reportId);
    void templateSelected(int index);

public slots:
    bool exportReport(const QString& path);
    bool exportHistory(const QString& path);

private:
    void buildUi();
    void rebuildTemplateTable();
    void rebuildDataSourceTable();
    void rebuildHistoryTable();
    void updatePreview();

    QWidget* containerWidget_ = nullptr;
    QTableWidget* templateTable_ = nullptr;
    QTableWidget* dataSourceTable_ = nullptr;
    QTableWidget* historyTable_ = nullptr;
    QTextEdit* previewView_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QPushButton* generateBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    QPushButton* exportHistoryBtn_ = nullptr;
    QTabWidget* tabs_ = nullptr;

    QVector<ReportTemplate> reportTemplates_;
    QVector<DataSource> dataSources_;
    QVector<ReportRecord> history_;
    int selectedTemplate_ = -1;
    QString selectedFormat_ = "HTML";
};
