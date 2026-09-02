#pragma once

// MasterComparisonWidget — side-by-side comparison of two EtherCAT masters.
// Highlights differences, supports merge, and conflict resolution.

#include <QWidget>

class QTableWidget;
class QLabel;
class QComboBox;
class QPushButton;
struct MmMasterInfo;
struct MmMasterStatus;

class MasterComparisonWidget : public QWidget {
    Q_OBJECT
public:
    explicit MasterComparisonWidget(QWidget* parent = nullptr);

    void setLeftMaster(const MmMasterInfo& info, const MmMasterStatus& status);
    void setRightMaster(const MmMasterInfo& info, const MmMasterStatus& status);
    void clearComparison();
    int differenceCount() const;

signals:
    void mergeRequested(int sourceId, int targetId);

private:
    void buildUi();
    void updateDiffHighlight();

    QComboBox* leftCombo_ = nullptr;
    QComboBox* rightCombo_ = nullptr;
    QTableWidget* leftTable_ = nullptr;
    QTableWidget* rightTable_ = nullptr;
    QTableWidget* diffTable_ = nullptr;
    QLabel* diffCountLabel_ = nullptr;
    QPushButton* mergeBtn_ = nullptr;
    int leftMasterId_ = -1;
    int rightMasterId_ = -1;
};
