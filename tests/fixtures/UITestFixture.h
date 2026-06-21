#pragma once

/// @brief Test fixture for UI widget tests with offscreen rendering support.
///
/// @details UITestFixture provides a managed environment for testing QWidget-
/// based components. It handles QApplication initialization for offscreen
/// rendering (required for headless CI environments) and tracks created
/// widgets for automatic cleanup.
///
/// Usage:
/// @code
///   UITestFixture::initOffscreen();  // Call once in initTestCase()
///   UITestFixture fixture;
///   auto *widget = fixture.createWidget();
///   QVERIFY(fixture.widgetExists(widget));
///   fixture.destroyWidget(widget);
///   QVERIFY(!fixture.widgetExists(widget));
/// @endcode
///
/// @par Test Coverage
///   - Widget creation and destruction
///   - Widget existence checking
///   - Offscreen QApplication initialization
///
/// @par Dependencies
///   - Qt6::Widgets (for QApplication and QWidget)
///
/// @note Call initOffscreen() once before any widget tests, typically in
///       the test class's initTestCase() method.

#include <QObject>
#include <QWidget>

class QApplication;

class UITestFixture : public QObject {
    Q_OBJECT
public:
    /// Constructs the fixture with an empty widget tracking list.
    explicit UITestFixture(QObject *parent = nullptr);
    /// Destroys the fixture and all managed widgets.
    ~UITestFixture() override;

    /// Creates a new QWidget with optional parent, tracked for cleanup.
    QWidget *createWidget(QWidget *parent = nullptr);
    /// Destroys a tracked widget and removes it from the tracking list.
    void destroyWidget(QWidget *widget);
    /// Returns true if the widget is currently tracked by this fixture.
    bool widgetExists(QWidget *widget) const;

    /// Initializes QApplication for offscreen rendering (call once per test suite).
    static void initOffscreen();

private:
    QVector<QWidget *> managedWidgets_; ///< List of widgets tracked for cleanup
};
