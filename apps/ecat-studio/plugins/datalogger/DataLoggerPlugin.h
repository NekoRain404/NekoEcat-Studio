#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QMap>
#include <QVariantMap>
#include <QVector>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class DataLoggerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit DataLoggerPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    struct LogFilter {
        QString name;
        QString source;
        QString level;
        bool enabled;
    };

    struct LogEntry {
        QDateTime timestamp;
        QString source;
        QString level;
        QString message;
        QVariantMap metadata;
    };

    struct LogFile {
        QString path;
        qint64 sizeBytes;
        QDateTime createdAt;
        int entryCount;
    };

    void addFilter(const LogFilter& filter);
    void removeFilter(int index);
    int filterCount() const;

    void addLogEntry(const LogEntry& entry);
    int logEntryCount() const;

    void searchLogs(const QString& query);
    void filterByLevel(const QString& level);

    void clearLogEntries();

    void addLogFile(const LogFile& file);
    void removeLogFile(int index);
    int logFileCount() const;

    void setMaxFileSize(qint64 bytes);
    qint64 maxFileSize() const;

    void setMaxFileCount(int count);
    int maxFileCount() const;

    QString exportLogData() const;

    QMap<QString, int> getSourceStatistics() const;

    QTabWidget* tabs() const;
    QTableWidget* filtersTable() const;
    QTableWidget* logFilesTable() const;
    QTextEdit* logViewer() const;
    QTableWidget* statisticsTable() const;
    QLabel* statusLabel() const;

signals:
    void logEntryAdded(const LogEntry& entry);
    void logFileRotated(const QString& path);

private:
    void buildUi();
    void rebuildFiltersTable();
    void rebuildLogFilesTable();
    void rebuildStatisticsTable();
    void updateLogViewer();

    QWidget* containerWidget_ = nullptr;
    QTabWidget* tabs_ = nullptr;
    QTableWidget* filtersTable_ = nullptr;
    QTableWidget* logFilesTable_ = nullptr;
    QTextEdit* logViewer_ = nullptr;
    QTableWidget* statisticsTable_ = nullptr;
    QLineEdit* searchEdit_ = nullptr;
    QComboBox* levelFilterCombo_ = nullptr;
    QPushButton* addFilterBtn_ = nullptr;
    QPushButton* removeFilterBtn_ = nullptr;
    QPushButton* toggleFilterBtn_ = nullptr;
    QPushButton* refreshFilesBtn_ = nullptr;
    QPushButton* deleteFileBtn_ = nullptr;
    QPushButton* refreshStatsBtn_ = nullptr;
    QLineEdit* maxSizeEdit_ = nullptr;
    QLineEdit* maxFilesEdit_ = nullptr;
    QLabel* statusLabel_ = nullptr;

    QVector<LogFilter> filters_;
    QVector<LogEntry> logEntries_;
    QVector<LogFile> logFiles_;
    qint64 maxFileSize_ = 10485760;
    int maxFileCount_ = 10;
    QString activeLevelFilter_;
    QString activeSearchQuery_;
};
